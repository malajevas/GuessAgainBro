import React, { useState, useEffect } from 'react';

const ATTEMPTS = 6;

function App() {
  const [sessionId, setSessionId] = useState('');
  const [inputText, setInputText] = useState('');
  const [error, setError] = useState(null);
  const [gameOverMessage, setGameOverMessage] = useState('');
  const [guesses, setGuesses] = useState([]);
  const [isActive, setIsActive] = useState(true);

  // Start new game on first load
  useEffect(() => {
    const startGame = async () => {
      try {
        const res = await fetch('http://localhost:1337/game/start');
        const data = await res.json();
        setSessionId(data.session_id);
      } catch (e) {
        setError('Failed to start game');
      }
    };

    startGame();
  }, []);

  const handleInputChange = (e) => {
    setInputText(e.target.value.toLowerCase());
  };

  const handleSubmit = async () => {
    if (!inputText || inputText.length === 0) return;
    if (!sessionId || !isActive) return;

    try {
      const res = await fetch('http://localhost:1337/game/word', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ session_id: sessionId, word: inputText }),
      });

      const data = await res.json();

      if (!res.ok) {
        setError(data.error || 'Unknown error');
        return;
      }

      const result = [];
      for (let i = 0; i < inputText.length; i++) {
        result.push({
          letter: inputText[i],
          status: data[i.toString()],
        });
      }

      setGuesses((prev) => [...prev, result]);

      if (data.message) {
        setGameOverMessage(data.message);
        setIsActive(false);
      }

      setInputText('');
      setError(null);
    } catch (e) {
      setError('Failed to send guess');
    }
  };

  const handleRestart = async () => {
    setGuesses([]);
    setGameOverMessage('');
    setError(null);
    setIsActive(true);
    setInputText('');

    try {
      const res = await fetch('http://localhost:1337/game/start');
      const data = await res.json();
      setSessionId(data.session_id);
    } catch (e) {
      setError('Failed to restart game');
    }
  };

  const getBackgroundColor = (status) => {
    if (status === 1) return 'green';
    if (status === 2) return 'goldenrod';
    return 'gray';
  };

  return (
    <div style={{ fontFamily: 'sans-serif', textAlign: 'center', padding: '20px' }}>
      <h1>Wordle Clone</h1>

      {guesses.length > 0 && (
        <div style={{ marginBottom: '20px' }}>
          {guesses.map((guess, rowIndex) => (
            <div key={rowIndex} style={{ marginBottom: '10px' }}>
              {guess.map((item, i) => (
                <span
                  key={i}
                  style={{
                    display: 'inline-block',
                    padding: '10px',
                    marginRight: '5px',
                    fontSize: '20px',
                    backgroundColor: getBackgroundColor(item.status),
                    color: 'white',
                    borderRadius: '5px',
                    minWidth: '30px',
                    textAlign: 'center',
                    textTransform: 'uppercase',
                  }}
                >
                  {item.letter}
                </span>
              ))}
            </div>
          ))}
        </div>
      )}

      {isActive && (
        <>
          <input
            type="text"
            value={inputText}
            onChange={handleInputChange}
            placeholder="Enter a word"
            style={{ padding: '8px', margin: '10px', fontSize: '16px' }}
          />
          <button
            onClick={handleSubmit}
            style={{ padding: '8px 15px', fontSize: '16px', cursor: 'pointer' }}
          >
            Guess
          </button>
        </>
      )}

      {!isActive && (
        <div style={{ marginTop: '20px' }}>
          <h2>{gameOverMessage}</h2>
          <button
            onClick={handleRestart}
            style={{
              padding: '8px 15px',
              fontSize: '16px',
              cursor: 'pointer',
              backgroundColor: '#007bff',
              color: 'white',
              border: 'none',
              borderRadius: '5px',
              marginTop: '10px',
            }}
          >
            Play Again
          </button>
        </div>
      )}

      {error && <div style={{ color: 'red', marginTop: '10px' }}>Error: {error}</div>}
    </div>
  );
}

export default App;

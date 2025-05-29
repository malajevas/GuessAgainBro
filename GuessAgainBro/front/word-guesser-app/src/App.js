import React, { useState, useEffect } from 'react';
import './App.css';

const ATTEMPTS = 6;
const WORD_LENGTH = 5;

function App() {
  const [sessionId, setSessionId] = useState('');
  const [inputText, setInputText] = useState('');
  const [error, setError] = useState(null);
  const [gameOverMessage, setGameOverMessage] = useState('');
  const [guesses, setGuesses] = useState([]);
  const [isActive, setIsActive] = useState(true);
  const [stats, setStats] = useState(null);
  const [showStats, setShowStats] = useState(false);
  const [startTime, setStartTime] = useState(null);
  const [elapsedTime, setElapsedTime] = useState(0);
  const [targetWord, setTargetWord] = useState('');

  // Start new game on first load
  useEffect(() => {
    const startGame = async () => {
      try {
        const res = await fetch('http://localhost:1337/game/start');
        const data = await res.json();
        setSessionId(data.session_id);
        setStartTime(Date.now());
      } catch (e) {
        setError('Failed to start game');
      }
    };

    startGame();
  }, []);
  
  useEffect(() => {
    let intervalId;

    if (showStats && isActive && startTime) {
      intervalId = setInterval(() => {
        setElapsedTime(Math.floor((Date.now() - startTime) / 1000));
      }, 1000);
    } else {
      setElapsedTime((prev) => prev);
    }

    return () => {
      if (intervalId) {
        clearInterval(intervalId);
      }
    };
  }, [showStats, isActive, startTime]);

  useEffect(() => {
    if (!isActive && startTime) {
      setElapsedTime(Math.floor((Date.now() - startTime) / 1000));
      setShowStats(true);
    }
  }, [isActive, startTime]);

  const handleInputChange = (e) => {
    const value = e.target.value.toLowerCase();
    if (value.length <= WORD_LENGTH) {
      setInputText(value);
    }
  };

  const handleSubmit = async () => {
    if (!inputText || inputText.length !== WORD_LENGTH) return;
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

      const statsRes = await fetch(`http://localhost:1337/game/stats?session_id=${sessionId}`);
      if (statsRes.ok) {
        const statsData = await statsRes.json();
        setStats(statsData);
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
        
        try {
          const wordRes = await fetch(`http://localhost:1337/game/word?session_id=${sessionId}`);
          if (wordRes.ok) {
            const wordData = await wordRes.json();
            setTargetWord(wordData.word || '');
          }
        } catch (e) {
          console.error('Failed to fetch target word');
        }
      }

      setInputText('');
      setError(null);
    } catch (e) {
      setError('Failed to send guess');
    }
  };

  const handleShowStats = async () => {
    if (!sessionId) return;

    if (showStats) {
      setShowStats(false);
      return;
    }

    try {
      const res = await fetch(`http://localhost:1337/game/stats?session_id=${sessionId}`);
      const data = await res.json();

      if (!res.ok) {
        setError(data.error || 'Failed to get stats');
        return;
      }

      setStats(data);
      setShowStats(true);
    } catch (e) {
      setError('Error fetching stats');
    }
  };

  const handleRestart = async () => {
    setGuesses([]);
    setGameOverMessage('');
    setError(null);
    setIsActive(true);
    setInputText('');
    setShowStats(false);
    setTargetWord('');

    try {
      const res = await fetch('http://localhost:1337/game/start');
      const data = await res.json();

      setSessionId(data.session_id);
      setStartTime(Date.now());
    } catch (e) {
      setError('Failed to restart game');
    }
  };

  const getCellClass = (status) => {
    let cellClass = 'grid-cell';
    if (status === 1) cellClass += ' correct';
    else if (status === 2) cellClass += ' present';
    else if (status !== null) cellClass += ' absent';
    return cellClass;
  };

  const renderGrid = () => {
    const grid = [];
    
    for (let row = 0; row < ATTEMPTS; row++) {
      const rowCells = [];
      
      for (let col = 0; col < WORD_LENGTH; col++) {
        let letter = '';
        let status = null;
        
        if (guesses[row] && guesses[row][col]) {
          letter = guesses[row][col].letter;
          status = guesses[row][col].status;
        }
        
        rowCells.push(
          <div
            key={`${row}-${col}`}
            className={getCellClass(status)}
          >
            {letter}
          </div>
        );
      }
      
      grid.push(
        <div key={row} className="grid-row">
          {rowCells}
        </div>
      );
    }
    
    return grid;
  };

  return (
    <div className="App">
      <h1>Wordle Clone</h1>

      <div className="grid">
        {renderGrid()}
      </div>

      {isActive && (
        <div>
          <input
            type="text"
            value={inputText}
            onChange={handleInputChange}
            placeholder="Enter a 5-letter word"
            maxLength={WORD_LENGTH}
            onKeyPress={(e) => {
              if (e.key === 'Enter') {
                handleSubmit();
              }
            }}
          />
          <br />
          <button
            onClick={handleSubmit}
            disabled={inputText.length !== WORD_LENGTH}
          >
            Submit Guess
          </button>

          <button
            onClick={handleShowStats}
            className="stats"
          >
            {showStats ? 'Hide Stats' : 'Show Stats'}
          </button>
        </div>
      )}

      {!isActive && (
        <div>
          <h2>{gameOverMessage}</h2>
          {targetWord && (
            <p>
              <strong>The word was: </strong>
              <div className="target-word">{targetWord}</div>
            </p>
          )}
          <button onClick={handleRestart}>
            Play Again
          </button>
        </div>
      )}

      {showStats && stats && (
        <div className="stats-box">
          <h3>Game Statistics</h3>
          <p><strong>Attempts Left:</strong> {ATTEMPTS - stats.attempts}</p>
          <p><strong>Time Played:</strong> {elapsedTime} seconds</p>
        </div>
      )}

      {error && (
        <div className="error-message">
          Error: {error}
        </div>
      )}
    </div>
  );
}

export default App;

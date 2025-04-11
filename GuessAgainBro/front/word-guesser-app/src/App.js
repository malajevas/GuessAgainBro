import React, { useState } from 'react';

function App() {
  const [inputText, setInputText] = useState('');
  const [response, setResponse] = useState(null);
  const [error, setError] = useState(null);

  const handleInputChange = (event) => {
    setInputText(event.target.value);
  };

  const handleSubmit = async () => {
    setError(null);
    setResponse(null);

    try {
      const res = await fetch('http://localhost:1337/game/word', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ word: inputText }),
      });

      if (!res.ok) {
        const errorData = await res.json();
        setError(`HTTP error! status: ${res.status}, message: ${errorData?.error || 'Unknown error'}`);
        return;
      }

      const data = await res.json();
      setResponse(data);
    } catch (e) {
      setError('Failed to send the request to the server.');
      console.error('Fetch error:', e);
    }
  };

  return (
    <div style={{ fontFamily: 'sans-serif', textAlign: 'center', padding: '20px' }}>
      <h1>Word Guesser</h1>
      <input
        type="text"
        value={inputText}
        onChange={handleInputChange}
        placeholder="Enter a word"
        style={{ padding: '8px', margin: '10px', fontSize: '16px' }}
      />
      <button onClick={handleSubmit} style={{ padding: '8px 15px', fontSize: '16px', cursor: 'pointer' }}>
        Guess Word
      </button>

      {error && (
        <div style={{ color: 'red', marginTop: '10px' }}>Error: {error}</div>
      )}

      {response && (
        <div style={{ marginTop: '20px' }}>
          <h2>Response from Server:</h2>
          <div>
            {inputText.split('').map((letter, index) => {
              const key = (index + 1).toString();
              let backgroundColor = 'lightgray';
              if (response[key] === 0) {
                backgroundColor = 'red';
              } else if (response[key] === 2) {
                backgroundColor = 'yellow';
              } else if (response[key] === 1) {
                backgroundColor = 'green';
              }
              return (
                <span
                  key={index}
                  style={{
                    display: 'inline-block',
                    padding: '10px',
                    marginRight: '5px',
                    fontSize: '20px',
                    backgroundColor: backgroundColor,
                    color: 'white',
                    borderRadius: '5px',
                    minWidth: '30px',
                    textAlign: 'center',
                  }}
                >
                  {letter}
                </span>
              );
            })}
          </div>
          <pre style={{ marginTop: '10px' }}>{JSON.stringify(response, null, 2)}</pre>
        </div>
      )}
    </div>
  );
}

export default App;
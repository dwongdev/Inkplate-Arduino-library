const express = require('express');
const fetch = require('node-fetch');
const app = express();

const CLIENT_ID = '396d5f95bfb741219a5404a16b4c421e';
const CLIENT_SECRET = '9e80e6ba89ab42a68779e79952dd192d';
const REDIRECT_URI = 'http://127.0.0.1:8888/callback';

app.get('/login', (req, res) => {
  const scope = 'user-read-playback-state';
  const authUrl =
    'https://accounts.spotify.com/authorize?' +
    new URLSearchParams({
      response_type: 'code',
      client_id: CLIENT_ID,
      scope,
      redirect_uri: REDIRECT_URI
    });
  res.redirect(authUrl);
});

app.get('/callback', async (req, res) => {
  const code = req.query.code;

  const tokenRes = await fetch('https://accounts.spotify.com/api/token', {
    method: 'POST',
    headers: {
      'Authorization':
        'Basic ' +
        Buffer.from(CLIENT_ID + ':' + CLIENT_SECRET).toString('base64'),
      'Content-Type': 'application/x-www-form-urlencoded'
    },
    body: new URLSearchParams({
      grant_type: 'authorization_code',
      code,
      redirect_uri: REDIRECT_URI
    })
  });

  const data = await tokenRes.json();
  res.send(
    `<pre>${JSON.stringify(data, null, 2)}</pre>
     <p>Copy the <b>refresh_token</b> and paste it into your Inkplate sketch.</p>`
  );

  console.log(data);
});

app.listen(8888, () => {
  console.log('Open http://localhost:8888/login');
});

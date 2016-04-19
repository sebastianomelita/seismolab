// =====================================
// Author: Rocco Musolino - @roccomuso =
// =====================================
// grab the packages we need
var express = require('express');
var app = express();
var port = 8080;

var bodyParser = require('body-parser');
app.use(bodyParser.json()); // support json encoded bodies
app.use(bodyParser.urlencoded({ extended: true })); // support encoded bodies


// ====================================
// ROUTES =============================
// ====================================

app.post('/seismocloud/alive.php', function(req, res) {
  console.log('---- New Request:')
  console.log('Headers:', req.headers);
  console.log('Body:', req.body);
  res.status(200).send('{"server":"www.sapienzaapps.it","ntpserver":"195.46.37.22","script":"","path":""}');
});

app.post('/seismocloud/terremoto.php', function(req, res) {
  console.log('---- New Request:')
  console.log('Headers:', req.headers);
  console.log('Body:', req.body);
  res.status(200).send('5');
});

// catch all the other requests.
app.all('*', function (req, res) {
  console.log(req.method, req.path, 'params:', req.params);
  console.log('body:', req.body);
  res.status(200).json({status: 'ok'});
});

// start the server
app.listen(port);
console.log('Server started! At http://localhost:' + port);

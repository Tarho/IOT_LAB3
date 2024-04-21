const express = require("express");
const bodyParser = require('body-parser');

const app = express();
const port = 8080;

app.use(bodyParser.json());

app.post('/api/lightconbuom', (req, res) => {
    const lightOn = req.body.light; // Access light directly
    const temperature = req.body.temperature; // Access temperature directly

    const response = {
        error: false,
        message: 'Data received successfully',
        data: {
            light: lightOn,
            temperature: temperature
        }
    };
    res.status(200).json(response);
});

app.listen(port, () => {
    console.log(`Server running on port ${port}`);
});

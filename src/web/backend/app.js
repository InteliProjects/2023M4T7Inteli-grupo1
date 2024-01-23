const mqtt = require('mqtt');
const fs = require('fs');
const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const db = require('./data/db');

const app = express();
const portApp = 8081;

app.use(cors());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.json());

app.listen(portApp, () => {
    console.log(`Servidor rodando em http://localhost:${portApp}`);
});

const mqtt_server = "be247e754cbb4e339e7338c891441b48.s1.eu.hivemq.cloud";
const port = 8883;
const mqtt_user = "mauro_teste_mqtt";
const mqtt_pass = "#hLntL5qH4RKWg5";
const caFile = './ca_certificate.pem';

// topics
const topic1 = "BLE_connection";
const topic2 = "attend_emergency";
const topic3 = "create_emergency";
const topic4 = "create_emergency_webApp";

const topics = [topic1, topic2, topic3, topic4];

// Parametros de conexão MQTT
const options = {
    clientId: 'mqttjs_' + Math.random().toString(16).substr(2, 8),
    username: mqtt_user,
    password: mqtt_pass,
    port: port,
    protocol: 'mqtts', // MQTT over TLS
    ca: [fs.readFileSync(caFile)], // Certificado CA
    connectTimeout: 60000,
};

// MQTT connection
const client = mqtt.connect(`mqtt://${mqtt_server}`, options);

client.on('connect', () => {
    console.log('Connected to MQTT broker');

    topics.forEach((topic) => {
        client.subscribe(topic);
        console.log(`Subscribed to topic: ${topic}`);
    });
});

// Errors
client.on('error', (err) => {
    console.error('Error:', err);
});

// Format incoming messages
client.on('message', (topic, message) => {
    const storedMessage = message.toString();
    console.log(`Received message on topic ${topic}: ${storedMessage}`);

    if (topic === "BLE_connection") {
        handleBLEConnection(storedMessage);
    } else if (topic === "attend_emergency") {
        handleAttendEmergency(storedMessage);
    } else if (topic === "create_emergency") {
        handleCreateEmergency(storedMessage);
    }
});

function handleBLEConnection(message) {
    console.log('Handling BLE Connection:', message);

    const [name, local] = message.split(',');
    const lastUpdate = new Date();

    db.run("UPDATE tbl_cli_pager SET local = ?, last_update = ? WHERE name = ?", [local, lastUpdate, name], (err) => {
        if (err) {
            console.log('Erro:', err);
        }
        console.log('Localização atual do pager atualizada com sucesso!');
    });
};

function handleAttendEmergency(message) {
    console.log('Handling Attend Emergency:', message);

    const attendTime = new Date();

    db.run(`UPDATE tbl_emergencies SET status = 'atendida', attend_time = ? WHERE id = (SELECT MAX(id) FROM tbl_emergencies)`, [attendTime], (err) => {
        if (err) {
            console.error('Erro ao fechar a última emergência:', err.message);
            return;
        }

        if (this.changes > 0) {
            console.log('Última emergência fechada com sucesso.');
        } else {
            console.log('Nenhuma emergência aberta encontrada.');
        }
    });
};

function handleCreateEmergency(message) {
    console.log('Handling Create Emergency:', message);

    const name = message;
    const status = "aberta";
    const creationTime = new Date();

    db.run(`INSERT INTO tbl_emergencies (name, emerg, local, level, status, creation_time) VALUES (?, ?, ?, ?, ?, ?)`, [name, null, null, null, status, creationTime], (err) => {
        if (err) {
            console.error('Erro ao inserir nova emrgência:', err.message);
            return;
        }
        console.log(`Nova emergência adicionada com o ID: ${this.lastID}`);

        db.get(`SELECT * FROM tbl_cli_pager WHERE name = ?`, [name], (err, row) => {
            if (err) {
                console.error('Erro ao consultar tbl_cli_pager:', err.message);
                return;
            }
            if (row) {
                console.log(row)
                db.run(`UPDATE tbl_emergencies SET local = ? WHERE name = ?`, [row.local, row.name], (err) => {
                    if (err) {
                        console.error('Erro ao atualizar local na tbl_emergencies:', err.message);
                        return;
                    }
                    console.log(`Local atualizado na tbl_emergencies para o dispositivo: ${row.name} que é o mesmo de ${name}`);
                });
            } else {
                console.log(`Nenhum registro ativo encontrado na tbl_cli_pager para o nome: ${name}`);
            }
        });
    });
};

app.get('/get_pager_data', (req, res) => {
    db.all("SELECT * FROM tbl_cli_pager", [], (err, rows) => {
        if (err) {
            console.error('Erro ao buscar dados do pager:', err.message);
            return res.status(500).send('Erro interno ao buscar dados do pager');
        }
        res.json(rows);
    });
});

app.post('/create_emergency', (req, res) => {
    const { emergency, location, severity } = req.body;
    const status = "aberta";
    const creationTime = new Date();

    db.run(`INSERT INTO tbl_emergencies (name, emerg, local, level, status, creation_time) VALUES (?, ?, ?, ?, ?, ?)`,
        [null, emergency, location, severity, status, creationTime], (err) => {
            if (err) {
                console.error('Erro ao inserir a nova emergência (geral achou paia :/):', err.message);
                return res.status(500).json({ error: 'Erro interno ao criar emergência' });
            }

            const emergencyId = this.lastID;
            res.json({ id: emergencyId, status: 'aberta' });

            const mqttMessage = `${emergency},${location},${severity}`;
            client.publish('create_emergency_webApp', mqttMessage);
            console.log('Mensagem MQTT enviada para create_emergency_webApp:', mqttMessage);
        });
});

app.get('/get_emergencies_data', (req, res) => {
    db.all("SELECT * FROM tbl_emergencies", [], (err, rows) => {
        if (err) {
            console.error('Erro ao buscar dados das emergências:', err.message);
            return res.status(500).send('Erro interno ao buscar dados das emergências');
        }
        res.json(rows);
    });
});

// Desconexão com o broker
process.on('SIGINT', () => {
    console.log('Disconnecting from MQTT broker');
    client.end(() => {
      process.exit();
    });
});
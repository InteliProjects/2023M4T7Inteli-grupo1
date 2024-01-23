const sqlite3 = require('sqlite3').verbose();

const DBSOURCE = "data/db.sqlite";

let db = new sqlite3.Database(DBSOURCE, (err) => {
    if (err) {
        console.error(err.message);
        throw err;
    } else {
        console.log('Conexão com o banco de dados estabelecida.')
        db.run(`CREATE TABLE IF NOT EXISTS tbl_cli_pager (
                id INTEGER PRIMARY KEY,
                name STRING,
                local STRING,
                status STRING,
                last_update DATETIME)`,
        (err) => {
            if (err) {
                console.log('Erro ao criar tbl_cli_pager:', err.message);
            } else {
                console.log('tbl_cli_pager criada com sucesso!');
            } 
        });

        db.run(`CREATE TABLE IF NOT EXISTS tbl_emergencies (
                id INTEGER PRIMARY KEY,
                name STRING,
                emerg STRING,
                local STRING,
                level INTEGER,
                status STRING,
                creation_time DATETIME,
                attend_time DATETIME)`,
        (err) => {
            if (err) {
                console.log('Erro ao criar tbl_emergencies:', err.message);
            } else {
                console.log('tbl_emergencies criada com sucesso!');
            }
        });
    };
});

module.exports = db;
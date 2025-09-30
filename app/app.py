from flask import Flask, render_template, redirect, url_for
import sqlite3
import paho.mqtt.client as mqtt
import json

app = Flask(__name__)

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC = "iot/mottuspot/comando"
MQTT_STATUS_TOPIC = "iot/mottuspot/status"

def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode())
        print(f"Status recebido do ESP32: {payload}")
        
        placa = payload.get("placa")
        status = payload.get("status")
        if placa and status:
            query_db("UPDATE motos SET status=? WHERE placa=?", (status, placa))
            print(f"Status da moto {placa} atualizado para: {status}")
    except Exception as e:
        print(f"Erro ao processar mensagem MQTT: {e}")

mqtt_client = mqtt.Client()
mqtt_client.on_message = on_message
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
mqtt_client.subscribe(MQTT_STATUS_TOPIC)
mqtt_client.loop_start()

def query_db(query, args=(), one=False):
    conn = sqlite3.connect("motos.db")
    conn.row_factory = sqlite3.Row
    cur = conn.cursor()
    cur.execute(query, args)
    rv = cur.fetchall()
    conn.commit()
    conn.close()
    return (rv[0] if rv else None) if one else rv

# Inicializar tabela - chamada automaticamente
def init_db():
    query_db("""CREATE TABLE IF NOT EXISTS motos (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        placa TEXT,
        descricao TEXT,
        status TEXT
    )""")
    if not query_db("SELECT * FROM motos"):
        query_db("INSERT INTO motos (placa, descricao, status) VALUES (?,?,?)",
                 ("ABC-1234", "Moto vermelha", "desativado"))
        query_db("INSERT INTO motos (placa, descricao, status) VALUES (?,?,?)",
                 ("XYZ-5678", "Moto azul", "desativado"))
        query_db("INSERT INTO motos (placa, descricao, status) VALUES (?,?,?)",
                 ("JKL-9999", "Moto preta", "desativado"))

# Inicializar o banco automaticamente quando o módulo for importado
init_db()

@app.route("/")
def index():
    motos = query_db("SELECT * FROM motos")
    return render_template("index.html", motos=motos)

@app.route("/motos/<int:moto_id>/<acao>")
def controlar_moto(moto_id, acao):
    moto = query_db("SELECT * FROM motos WHERE id=?", (moto_id,), one=True)
    if not moto:
        return "Moto não encontrada", 404

    if acao not in ["ativar", "desativar"]:
        return "Ação inválida", 400

    query_db("UPDATE motos SET status=? WHERE id=?", (acao, moto_id))

    # Criar comando JSON compatível com ESP32
    comando = {
        "placa": moto["placa"],
        "descricao": moto["descricao"],
        "status": acao
    }

    # Enviar comando como JSON string
    mqtt_client.publish(MQTT_TOPIC, json.dumps(comando))
    
    print(f"📡 Comando MQTT enviado: {json.dumps(comando)}")

    return redirect(url_for("index"))

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)

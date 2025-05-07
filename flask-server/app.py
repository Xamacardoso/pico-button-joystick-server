from flask import Flask, request, jsonify, render_template
from datetime import datetime

app = Flask(__name__)
data = {
    "btn_a": 0,
    "btn_b": 0,
    "temp": 0.0,
    "joy_x": 0,
    "joy_y": 0,
    "direction": "Centro",
    "timestamp": "N/A"
}

def compute_direction(x, y):
    if y > 60:
        if x < 40:
            return "Noroeste"
        elif x > 60:
            return "Nordeste"
        else:
            return "Norte"
    elif y < 40:
        if x < 40:
            return "Sudoeste"
        elif x > 60:
            return "Sudeste"
        else:
            return "Sul"
    else:
        if x < 40:
            return "Oeste"
        elif x > 60:
            return "Leste"
        else:
            return "Centro"

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/update")
def joystick():
    data["x"] = int(request.args.get("joy_x", 0))
    data["y"] = int(request.args.get("joy_y", 0))
    data["btn_a"] = int(request.args.get("btn_a", 0))
    data["btn_b"] = int(request.args.get("btn_b", 0))
    data["temp"] = int(request.args.get("temp", 0))
    
    x = data["x"]
    y = data["y"]
    
    # Computa a direção da "rosa dos ventos" com base nos valores de x e y
    data["direction"] = compute_direction(x, y)

    data["timestamp"] = datetime.now().strftime("%H:%M:%S")
    print(data)
    return "OK"

@app.route("/data")
def json_data():
    return jsonify(data)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)

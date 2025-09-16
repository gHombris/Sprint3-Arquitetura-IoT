from flask import Flask, request, jsonify

# Cria a aplicação web
app = Flask(__name__)

# Define a rota (endpoint) que vai "ouvir" os resultados do treino
@app.route('/treino/resultado', methods=['POST'])
def receber_resultado():
    # Pega os dados JSON que foram enviados
    dados = request.get_json()

    # Verifica se a chave "toques" existe nos dados
    if not dados or 'toques' not in dados:
        return jsonify({"status": "erro", "mensagem": "Dados invalidos"}), 400

    # Pega o número de toques
    numero_de_toques = dados['toques']

    # Imprime no terminal para mostrar que recebemos!
    print(f"✅ Resultado recebido do dispositivo! Toques: {numero_de_toques}")

    # Responde para quem enviou, confirmando o recebimento
    return jsonify({
        "status": "sucesso",
        "mensagem": f"{numero_de_toques} toques registrados com sucesso!"
    }), 200

# Roda o servidor
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
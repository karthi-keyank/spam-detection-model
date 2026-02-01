from flask import Flask, request, jsonify
import joblib
import re

# ------------------------------
# 1. Load model ONCE at startup
# ------------------------------
try:
    model = joblib.load("models/spam_model2.pkl")
except Exception as e:
    raise RuntimeError(f"Failed to load model: {e}")

app = Flask(__name__)

# ------------------------------
# 2. Text cleaning (MUST match training)
# ------------------------------
def clean_text(text):
    if not isinstance(text, str):
        return ""

    text = text.lower()
    text = re.sub(r"http\S+|www\S+", " ", text)   # remove URLs
    text = re.sub(r"\d+", " ", text)              # remove numbers
    text = re.sub(r"[^a-z\s]", " ", text)         # remove symbols
    text = re.sub(r"(.)\1{2,}", r"\1", text)      # helloooo -> hello
    text = re.sub(r"\s+", " ", text)
    return text.strip()

# ------------------------------
# 3. Health check
# ------------------------------
@app.route("/", methods=["GET"])
def home():
    return jsonify({
        "status": "server running",
        "model_loaded": True
    })

# ------------------------------
# 4. Prediction endpoint
# ------------------------------
@app.route("/predict", methods=["POST"])
def predict():
    data = request.get_json(silent=True)

    if not data or "message" not in data:
        return jsonify({
            "error": "JSON body must contain 'message'"
        }), 400

    raw_message = data["message"]
    message = clean_text(raw_message)

    if message == "":
        return jsonify({
            "error": "Empty or invalid message"
        }), 400

    # Run model
    probs = model.predict_proba([message])[0]
    spam_prob = float(probs[1])

    # ------------------------------
    # Decision logic (IMPORTANT)
    # ------------------------------
    if spam_prob >= 0.7:
        label = "spam"
        spam = 1
    elif spam_prob >= 0.5:
        label = "suspicious"
        spam = 0
    else:
        label = "not_spam"
        spam = 0

    return jsonify({
        "original_message": raw_message,
        "cleaned_message": message,
        "spam": spam,
        "label": label,
        "confidence": round(spam_prob, 3)
    })

# ------------------------------
# 5. Run server
# ------------------------------
if __name__ == "__main__":
    app.run(
        host="0.0.0.0",
        port=5000,
        debug=False
    )

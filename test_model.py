import joblib

model = joblib.load("models/spam_model2.pkl")

tests = [
    "Win free money now",
    "Hello how are you",
    "Limited offer click now",
    "Are we meeting tomorrow?"
]

for msg in tests:
    pred = model.predict([msg])[0]
    print(msg, "=>", "SPAM 🚨" if pred == 1 else "NOT SPAM ✅")

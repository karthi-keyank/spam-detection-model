import requests

# ------------------------------
# Server URL
# ------------------------------
SERVER_URL = "http://127.0.0.1:5000/predict"
# If testing from another device, use:
# SERVER_URL = "http://10.155.30.124:5000/predict"

print("🧪 Spam Detection Test Client")
print("Type a message to test")
print("Type 'exit' to quit\n")

while True:
    message = input("Enter message: ").strip()

    if message.lower() == "exit":
        print("Exiting 👋")
        break

    if not message:
        print("⚠️ Empty message, try again\n")
        continue

    try:
        response = requests.post(
            SERVER_URL,
            json={"message": message},
            timeout=5
        )

        if response.status_code != 200:
            print("❌ Server error:", response.text, "\n")
            continue

        result = response.json()

        spam = result["spam"]
        confidence = result["confidence"]

        if spam == 1:
            print(f"🚨 SPAM  | confidence = {confidence}\n")
        else:
            print(f"✅ NOT SPAM | confidence = {confidence}\n")

    except requests.exceptions.RequestException as e:
        print("❌ Could not connect to server")
        print(e, "\n")

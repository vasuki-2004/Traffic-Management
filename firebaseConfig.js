const admin = require("firebase-admin")
// firebase
// initializeApp

const serviceAccount = require("./serviceAccountKey.json")

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: "https://nscet-iot-transport-default-rtdb.firebaseio.com/"
})

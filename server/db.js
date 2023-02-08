const mongoose = require('mongoose');

module.exports = () => {
    const connectionParams = {
        useNewUrlParser: true,
        useUnifiedTopology: true,   
    };
    try {
        mongoose.connect(process.env.DB_URL, connectionParams);
        console.log('Connected to database ');
    } catch(error) {

    }
}

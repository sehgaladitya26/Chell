const router = require('express').Router();
const { User, validate } = require('../models/user');
const bcrypt = require('bcrypt');


router.post("/", async(req, res) => {
    try {
        const { error } = validate(req.body);
        if (error) 
            return res.status(400).send(error.details[0].message);

        const user = await User.findOne({ email: req.body.email });
        if (user) 
            return res.status(409).send('User already exists.');

        const salt = await bcrypt.genSalt(Number(process.env.SALT));
        const hashedPassword = await bcrypt.hash(req.body.password, salt);

        const newUser = new User({...req.body, password: hashedPassword}).save();
        res.status(201).send({message: 'User created successfully.'});

    } catch(error) {
        return res.status(400).send(error.details[0].message);
    }
});
module.exports = router;


const router = require('express').Router();
const { User, validate } = require('../models/user');
const bcrypt = require('bcrypt');
const Joi = require('joi');

router.post("/", async(req, res) => {
    try {
        const { error } = validate(req.body);
        if (error) 
            return res.status(400).send(error.details[0].message);

        const user = await User.findOne({ email: req.body.email });
        if (user)
            return res.status(401).send('Invalid email or password.');

        const validPassword = await bcrypt.compare(req.body.password, user.password);
        if (!validPassword)
            return res.status(401).send('Invalid email or password.');

        const token = user.generateAuthToken();
        res.status(200).send({data:token, message: 'User logged in successfully.'});


    } catch(error) {
        return res.status(400).send(error.details[0].message);
    }
});

const validate = (data) => {
    const Schema = Joi.object({
        email: Joi.string().email().required().label('Email'),
        password: passwordComplexity().required().label('Password'),
    });
    return schema.validate(data);
}

module.exports = router;
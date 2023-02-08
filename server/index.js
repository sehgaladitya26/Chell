require("dotenv").config();
const express = require("express");
const cors = require("cors");

// middleware
app.use(express.json());
app.use(cors());

const port = process.env.PORT || 8080;
app.listen(port, () => console.log(`Server is running on port ${port}`));

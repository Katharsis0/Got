var express = require('express');
var router = express.Router();

/* GET users listing. */
router.get('/', function(req, res, next) {
  res.send('respond with a resource');
});

router.route('/users').get((req, res) => {
    user.find()
    .then(users => res.json(users))
    .catch(err => res.status(400).json('Error: ' + err));
});

router.route('/add').post((req, res) => {
    const username = req.body.username;
    const password = req.body.password;

    const newUser = new user({
        username, 
        password,
    });

    newUser.save()
    .then(() => res.json('New User Registered'))
    .catch(err => res.status(400).json('Error: ' + err));
});

module.exports = router;

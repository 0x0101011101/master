// Loading Express Module
const express = require('express');
// Create Express Router
var router = express.Router();

// Loading Mongoose Module
const mongoose = require('mongoose');
// Loading Employee Model
const Employee = mongoose.model('Employee');

// Manage Root
router.get('/', (req, res) => {
	res.render('employee/addOrEdit', {
		viewTitle: 'Insert Employee'
	});
});

// Insert Employee
router.post('/', (req,res) => {
	if (req.body._id == '')
		insertEmployee(req, res);
	else
		updateEmployee(req,res);
});

// Function Insert
function insertEmployee(req,res) {
	var employee = new Employee();
	employee.fullName = req.body.fullName;
	employee.email = req.body.email;
	employee.mobile = req.body.mobile;
	employee.city = req.body.city;

	// Saving record
	employee.save((err,doc) => {
		if(!err) {
			res.redirect('employee/list');
		}
		else {
			if(err.name == 'ValidationError') {
				handleValidationError(err,req.body);
				res.render('employee/addOrEdit', {
					viewTitle: 'Insert Employee',
					employee: req.body
				});
			}
			else
				console.log('Error during record insertion : ' + err);
		}
	});
}

// Update Employee
function updateEmployee(req,res) {
	Employee.findOneAndUpdate({
		_id:req.body._id
	},
	req.body,
	{
		new:true
	},
	(err,doc) => {
		if (!err) {
			res.redirect('employee/list');
		} else {
			if(err.name == 'ValidationError') {
				handleValidationError(err.req.body);
				res.render("employee/addOrEdit", {
					viewTitle: 'Update Employee',
					employee: req.body
				});
			} else {
				console.log('Error during Employee Update : ' + err);
			}
		}
	});
}

// Route for Employee List
router.get('/list', (req,res) => {
	//res.json('from list'); TO DEBUG
	Employee.find((err, docs) => {
		if(!err) {
			//console.log(docs);
			res.render('employee/list', {
				list: docs
			});

		} else {
			console.log('Error in retrieving Employee List : ' + err);
		}
	});
});

// Handling Error
function handleValidationError(err,body) {
	for(field in err.errors) {
		switch(err.errors[field].path) {
			case 'fullName':
				body['fullNameError'] = err.errors[field].message;
				break;
			case 'email':
				body['emailError'] = err.errors[field].message;
				break;
			default:
				break;
		}
	}
}

// Edit Employee passing id params (see MongoDB registered item. Can consider it is the PK of an item automatically created)
router.get('/:id', (req, res) => {
	Employee.findById(req.params.id, (err, doc) => {
		if (!err) {
			res.render('employee/addOrEdit', {
				viewTitle: 'Update Employee',
				employee: doc
			});
		}
	});
});

// Delete Employee
router.get('/delete/:id', (req,res) => {
	Employee.findByIdAndRemove(req.params.id, (err,doc) => {
		if (!err) {
			res.redirect('/employee/list');
		} else {
			console.log('Error during deleting Employee : ' + err);
		}
	});
});

// To make this module exportable (loaded in Server.js)
module.exports = router;
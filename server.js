// test.js
const addon = require('./build/Release/addon');

var express = require("express");
var bodyParser = require("body-parser");
 
var app = express();
 
// создаем парсер для данных application/x-www-form-urlencoded
var urlencodedParser = bodyParser.urlencoded({extended: false});
 
app.use(express.static(__dirname + "/public"));

//Вызов функции проверки пользователя 
app.post("/check", urlencodedParser, function (request, response) 
{
    if(!request.body) return response.sendStatus(400);
    console.log(request.body);
    response.send(`${addon.users(request.body.userText)}`);
});

//Вызов функции получения всех пользователей
app.post("/getall", urlencodedParser, function (request, response) 
{
    if(!request.body) return response.sendStatus(400);
    console.log(request.body);
    response.send(`${addon.getall()}`);
}); 
 
//Перенаправление на /main.html
app.get("/", function(request, response){
	console.log("Redirect to: " + request.headers.host + "/main.html");
    response.redirect("http://" + request.headers.host + "/main.html");
    //response.send("<h1>Главная страница</h1>");
});
 
app.listen(3000);
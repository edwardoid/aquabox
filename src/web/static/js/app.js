var app = ons.bootstrap('aquabox', ['onsen', 'ngStorage']);
var pc = undefined;
var TOKEN_KEY = 'aquabox-user-token'
var authenticated = false;

app
  .controller("DevicesListController", ['$scope', '$http', function ($scope, $http, $timeout) {
      var ts = this;
      this.devices = []

      this.load = function($done) {
        $http.get("/api/v1/devices").then(
          function successCallBack(response) {
            ts.devices = response.data.devices  
            if($done)
              $done();
          },
          function errorCallBack(response) {   
            if($done)
              $done();
          }
        )
      };
  }]);

app
  .controller('PageCtrl', ['$scope', '$http', function ($scope, $http) {
    pc = $scope;

    $scope.openMenu = function () {
      var menu = document.getElementById('menu');
      menu.open();
    };

    $scope.loadPage = function (page) {
      var menu = document.getElementById('menu');
      menu.close();
    };

    $scope.logout =function() {
      var menu = document.getElementById('menu');
      menu.close();
      $scope.authFail("");
    }

    $scope.authOk = function(token) {
      localStorage.setItem(TOKEN_KEY, token)
      authenticated = true;
      console.log("ok")
      navi.pushPage('main.html')
    };

    $scope.authFail = function(token) {
      localStorage.setItem(TOKEN_KEY, "")
      authenticated = false;
      navi.pushPage('login.html')
    };

    $scope.doAuth = function(token) {
      var Url = "/api/v1/login";
      var Body = token;
      $http.post(Url, Body).then(
        function successCallBack(response) {
          $scope.authOk(token)
        },
        function errorCallBack(response) {   
          $scope.authFail(token)
        }
      )
    };

    $scope.login = function () {
      var username = document.getElementById('username').value;
      var password = document.getElementById('password').value;
      var possibleToken = username + ":" + password;
      possibleToken = sha512(possibleToken);
      if (possibleToken.length > 0) {
        pc.doAuth(possibleToken)
      }
    };

    angular.element(document).ready(function () {
      if (authenticated)
        return;
      $scope.doAuth(localStorage.getItem(TOKEN_KEY))
    })

    document.addEventListener('init', function (event) {
    });
  }]);


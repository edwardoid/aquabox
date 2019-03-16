
function jumpToPage(target) {
  console.log("Going to");
  if (target === "main") {
    navi.pushPage('main.html')
  }
}

var login = function(){}

var bootstrap = ons.bootstrap(['ngStorage']);

bootstrap
  .controller('LoginController', function($scope, $localStorage) {
      var token = $localStorage.$default({
        token : "xxxx"
      });

      var initialPage = token.length > 0 ? "main.html" : "login.html"


      $scope.login = function() {
        $localStorage.token = "ZZZZ";
        navi.pushPage('main.html')
      }


      login = $scope.login
  });

bootstrap
  .controller('PageCtrl', function ($scope, $localStorage) {
    $scope.openMenu = function () {
      var menu = document.getElementById('menu');
      menu.open();
    };

    $scope.loadPage = function (page) {
      var menu = document.getElementById('menu');

      menu.close();
      $scope.navi.pushPage(page);
    };

    document.addEventListener('init', function (event) {
    });
  });
bootstrap
  .controller("DevicesListController", function ($scope, $http, $timeout) {
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
  });

document.addEventListener('init', function (event) {
  if (event.target.id === 'pageNav1') {
    var title = event.target.data && event.target.data.title ? event.target.data.title : 'Custom Page';
    event.target.querySelector('ons-toolbar div.center').textContent = title;
  }
  if (event.target.id === 'pageNav2') {
    var cardTitle = event.target.data && event.target.data.cardTitle ? event.target.data.cardTitle : 'Custom Card';
    event.target.querySelector('ons-card div.title').textContent = cardTitle;
  }
});

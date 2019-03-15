function jumpToPage(target) {
  console.log("Going to");
  if (target === "main") {
    navi.pushPage('main.html')
  }
}

ons.bootstrap()
  .controller('PageCtrl', function ($scope) {
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

var customPush = function () {
  navi.pushPage('pageNav1.html', { data: { title: navi.topPage.querySelector('ons-input').value } })
};

var customPush2 = function (event) {
  navi.pushPage('pageNav2.html', { data: { cardTitle: event.target.textContent } })
};

function login() {
  jumpToPage("main");
}
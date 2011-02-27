Drupal.behaviors.gitRepoViewer = function(context) {
  $('.repo-branches-form select#edit-branches').change(
    function(context) {
      var url = Drupal.gitRepoViewer.getBasePath() + '?branch=' + $(this).val();
      var path = '', commit = '';
      if (path = Drupal.gitRepoViewer.getQueryStrings('path')) {
        url = url + '&path=' + path;
      }
      if (commit = Drupal.gitRepoViewer.getQueryStrings('commit')) {
        url = url + '&commit=' + commit;
      }
      $(window.location).attr('href', url);
    }
  );
}

Drupal.gitRepoViewer = function() {
}

Drupal.gitRepoViewer.getBasePath = function() {
  var regex = "(.*)\\?";
  var regex = new RegExp(regex);
  var results = regex.exec(window.location.href);
  return results[1];
}

Drupal.gitRepoViewer.getQueryStrings = function(name) {
  name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
  var regex = "[\\?&]"+name+"=([^&#]*)";
  var regex = new RegExp(regex);
  var results = regex.exec(window.location.href);
  if (results == null) {
    return false;
  }
  else {
    return decodeURIComponent(results[1].replace(/\+/g, " "));
  }
}

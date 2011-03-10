// TODO: Make sure we're not loading and running unnecessary code because all 
// of our js is in this file.
Drupal.behaviors.gitRepoViewer = function(context) {
  $('.repo-branches-form select#edit-branches').change(
    function(context) {
      var url = location.protocol + '//' + location.hostname + location.pathname + '?branch=' + $(this).val();
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

Drupal.behaviors.gitRepoViewerCodeSelector = function(context) {
  $('.git-repo-browser-content .geshifilter ol').each(
    function () {
      var lineNumber = 1;
      $('li', this).each(
        function() {
          $(this).click(Drupal.gitRepoViewer.lineNumberClick);
          // TODO: Lets
          /*
          $(this).mousedown(Drupal.gitRepoViewer.lineNumberClick);
          $(this).mouseup(Drupal.gitRepoViewer.lineNumberClick);
          */
          $(this).data('gitRepoViewerLineNumber', lineNumber);
          lineNumber++;
        }
      );
    }
  );
}

Drupal.gitRepoViewer = function() {
}

Drupal.gitRepoViewer.lineNumberClick = function() {
  var clickedLineNumber = $(this).data('gitRepoViewerLineNumber');
  console.log(clickedLineNumber);
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

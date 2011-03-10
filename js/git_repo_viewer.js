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
          $(this).data('gitRepoViewerLineNumber', lineNumber).addClass('git-repo-line-' + lineNumber);
          lineNumber++;
        }
      );
    }
  );
  var token = $('#edit-filter-token').val();
  Drupal.gitRepoViewer.filterToken = token.substring(0, token.length - 1);
}

Drupal.gitRepoViewer = function() {
}

// TODO: make these not global, what happens if there are two browsers on one page?
Drupal.gitRepoViewer.lowNumber = 0;
Drupal.gitRepoViewer.highNumber = 0;
Drupal.gitRepoViewer.filterToken = '';

Drupal.gitRepoViewer.lineNumberClick = function() {

  var clickedLineNumber = $(this).data('gitRepoViewerLineNumber');

  // If we don't have a low or a high, set a low.
  if (Drupal.gitRepoViewer.lowNumber == 0 && Drupal.gitRepoViewer.highNumber == 0) {
    Drupal.gitRepoViewer.lowNumber = clickedLineNumber;
    // TODO: Make this configurable.
    $(this).css('background', '#769CE8');
  }
  // If we have a low but not a high, set the high.
  else if (Drupal.gitRepoViewer.lowNumber != 0 && Drupal.gitRepoViewer.highNumber == 0) {
    if (clickedLineNumber < Drupal.gitRepoViewer.lowNumber) {
      Drupal.gitRepoViewer.highNumber = Drupal.gitRepoViewer.lowNumber;
      Drupal.gitRepoViewer.lowNumber = clickedLineNumber;
    }
    else {
      Drupal.gitRepoViewer.highNumber = clickedLineNumber;
    }
    $('#edit-filter-token').val(Drupal.gitRepoViewer.filterToken + '-' + Drupal.gitRepoViewer.lowNumber + ':' + Drupal.gitRepoViewer.highNumber + ']');
    Drupal.gitRepoViewer.setSelection(Drupal.gitRepoViewer.lowNumber, Drupal.gitRepoViewer.highNumber);
  }
  // If we already have a low and a high, set the unset the selection.
  else if (Drupal.gitRepoViewer.lowNumber != 0 && Drupal.gitRepoViewer.highNumber != 0) {
    Drupal.gitRepoViewer.unsetSelection($(this).parent('.geshifilter ol'));
    $('#edit-filter-token').val(Drupal.gitRepoViewer.filterToken + ']');
  }
}

Drupal.gitRepoViewer.setSelection = function(start, end) {
  var i = start;
  while (i <= end) {
    // TODO: Make this configurable.
    $('.git-repo-line-' + i).css('background', '#769CE8');
    i++;
  }
}

Drupal.gitRepoViewer.unsetSelection = function(content) {
  Drupal.gitRepoViewer.lowNumber = 0;
  Drupal.gitRepoViewer.highNumber = 0;
  $('li', content).each(
    function() {
      $(this).css('background', 'none');
    }
  );
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

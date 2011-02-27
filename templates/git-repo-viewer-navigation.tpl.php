<?php
/**
 * @file
 */
?>
<div class="git-repo-nav">
  <?php if ($previous_commit_link || $next_commit_link): ?>
    <div class="commit-links">
      <?php if ($previous_commit_link): ?>
        <div class="previous-commit-link"><?php print $previous_commit_link; ?></div>
      <?php endif; ?>
      <?php if ($next_commit_link): ?>
        <div class="next-commit-link"><?php print $next_commit_link; ?></div>
      <?php endif; ?>
    </div>
  <?php endif; ?>
  <?php if ($parent_folder_link || $history): ?>
    <div class="folder-links">
      <?php if ($parent_folder_link): ?>
        <div class="parent-folder-link"><?php print $parent_folder_link; ?></div>
      <?php endif; ?>
      <?php if ($history): ?>
        <div class="history-link"><?php print $history; ?></div>
      <?php endif; ?>
    </div>
  <?php endif; ?>
  <?php if ($branches_form): ?>
    <div class="clear-block">
      <div class="repo-branches-form">
        <?php print $branches_form; ?>
      </div>
    </div>
  <?php endif; ?>
</div>

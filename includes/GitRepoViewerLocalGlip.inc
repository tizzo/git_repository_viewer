<?php

/**
 * @file
 *   
 * TODO: 
 */

interface GitRepoViewerInterface {

  /**
   * Get all relevant metadata about a repository object.
   */
  public function getRepoData($path, $branch, $commit = FALSE);

  /** 
   * Get a representation of an Object within the Git Repository.
   *
   * @param string $repo_location
   *   The path to the actual repository on disk.
   * @param string $path
   *   The path withihin the repository for which we are trying to retrieve an object.
   * @param string $branch
   *   The branch on which we are trying to view this object.
   *   TODO: This is here out of a vague suspicion that it'll be critical for svn
   *   (if this is abstracted that far).  Should it be?
   * @param string $commit
   *   The revision at which we are trying to view this object.
   *
   * @return mixed
   *   On failure, FALSE.
   *   On success, a stdClass object whose attributes are as follows:
   *     @code
   *        // The name of this element.
   *        $data->name '';
   *        // The type of this element, either file or directory.
   *        $data->type '';
   *        // The contents of this element, for files the content of the file for
   *        // directories a multidimmenstional array.
   *        $data->contents '';
   *     @endcode
   */
  public function getObject($repo_location, $path, $branch, $commit);

  /** 
   * Get a representation of an Object within the Git Repository.
   *
   * @param string $repo_location
   *   The path to the actual repository on disk.
   * @param string $branch
   *   The branch on which we are trying to view this object.
   *   TODO: This is here out of a vague suspicion that it'll be critical for svn
   *   (if this is abstracted that far).  Should it be?
   * @param string $commit
   *   The revision at which we are trying to view this object.
   *
   * @return string
   *   The contents of the commit.
   */
  public function getCommit($repo_location, $branch, $commit);

}

class GitRepoViewerLocalGlip implements GitRepoViewerInterface {
  
  public function __construct() {
  }

  /**
   * Get all relevant metadata about a repository object.
   */
  public function getRepoData($repo_location, $branch, $commit = FALSE) {
    $data = array();
    // Get a Glip repository object for this repository.
    $glip_repo = new Git($repo_location);

    // If we do not yet have a SHA-1 hash of the commit to view, find it now.
    if (!$commit) {
      $commit = sha1_hex($glip_repo->getTip($branch));
    }
    else {
      $commit = $commit;
    }
    $data['commit'] = $commit;
    
    $parents = array();
    try {
      foreach ($glip_repo->getObject(sha1_bin($commit))->parents as $parent) {
        $parents[] = sha1_hex($parent);
      }
    }
    catch (Exception $e) {
      $error = t('Object %sha1 could not be found in repository at path %path', array('%sha1' => sha1_hex($commit), '%path' => $repo_location));
      watchdog('git_repository_viewer', $error);
    }

    $data['parents'] = $parents;

    return $data;
  }

  /** 
   * Get a representation of an Object within the Git Repository.
   *
   * @param string $repo_location
   *   The path to the actual repository on disk.
   * @param string $path
   *   The path withihin the repository for which we are trying to retrieve an object.
   * @param string $branch
   *   The branch on which we are trying to view this object.
   *   TODO: This is here out of a vague suspicion that it'll be critical for svn
   *   (if this is abstracted that far).  Should it be?
   * @param string $commit
   *   The revision at which we are trying to view this object.
   *
   * @return mixed
   *   On failure, FALSE.
   *   On success, a stdClass object whose attributes are as follows:
   *     @code
   *        // The name of this element.
   *        $data->name '';
   *        // The type of this element, either file or directory.
   *        $data->type '';
   *        // The contents of this element, for files the content of the file for
   *        // directories a multidimmenstional array.
   *        $data->contents '';
   *     @endcode
   */
  public function getObject($repo_location, $path, $branch, $commit) {
    $commit = sha1_bin($commit);
    $data = new stdClass;
    $glip_repo = new Git($repo_location);
    try {
      $commit = $glip_repo->getObject($commit);
      // Get the tree for this repository at this commit.
      $root_tree = $glip_repo->getObject($commit->tree);
    }
    catch(Exception $e) {
      return FALSE;
    }
    $path = explode('/', $path);
    $object = $root_tree;
    $data->name = FALSE;
    if (count($path) && $path[0] != '') {
      foreach ($path as $item) {
        if (array_key_exists($item, $object->nodes)) {
          $item_hash = $object->nodes[$item]->object;
          $object = $glip_repo->getObject($item_hash);
          $data->name = $item;
        }
        else {
          return FALSE;
        }
      }
    }
    if (get_class($object) == 'GitTree') {
      $data->type = 'directory';
      $data->contents = array();
      foreach ($object->nodes as $name => $node) {
        // Here we deliberately abstract things slightly from the glip model
        // because we may want to use other libraries (or even non-git repos)
        // at some point.
        $data->contents[$name] = array(
          'name' => $name,
          'directory' => $node->is_dir,
          'mode' => $node->mode,
          'submodule' => $node->is_submodule,
          'object_id' => sha1_hex($node->object),
        );
      }
    }
    elseif (get_class($object) == 'GitBlob') {
      $data->type = 'file';
      $data->object_id = sha1_hex($item_hash);
      $data->name = $item;
      $data->contents = $object->data;
    }
    return $data;
  }

  /** 
   * Get a representation of an Object within the Git Repository.
   *
   * @param string $repo_location
   *   The path to the actual repository on disk.
   * @param string $branch
   *   The branch on which we are trying to view this object.
   *   TODO: This is here out of a vague suspicion that it'll be critical for svn
   *   (if this is abstracted that far).  Should it be?
   * @param string $commit
   *   The revision at which we are trying to view this object.
   *
   * @return string
   *   The contents of the commit.
   */
  public function getCommit($repo_location, $branch, $commit) {
    try {
      // TODO: This can almost certainly be done without changing directories.
      $cwd = getcwd();
      chdir($repo_location);
      $diff_command = "git show '{$commit}'";
      $output = array();
      $return = FALSE;
      $diff = shell_exec($diff_command);
      chdir($cwd);
      if (!$return) {
        return $diff;
      }
      else {
        return FALSE;
      }
    }
    catch (Exception $e) {
      return FALSE;
    }
  }
}

class GitRepoViewerRemoteGlip implements GitRepoViewerInterface {

  /** 
   *
   */
  public function __construct() {
  }

  /**
   * Get all relevant metadata about a repository object.
   */
  public function getRepoData($path, $branch, $commit = FALSE) {

  }

  /** 
   * Get a representation of an Object within the Git Repository.
   *
   * @param string $repo_location
   *   The path to the actual repository on disk.
   * @param string $path
   *   The path withihin the repository for which we are trying to retrieve an object.
   * @param string $branch
   *   The branch on which we are trying to view this object.
   *   TODO: This is here out of a vague suspicion that it'll be critical for svn
   *   (if this is abstracted that far).  Should it be?
   * @param string $commit
   *   The revision at which we are trying to view this object.
   *
   * @return mixed
   *   On failure, FALSE.
   *   On success, a stdClass object whose attributes are as follows:
   *     @code
   *        // The name of this element.
   *        $data->name '';
   *        // The type of this element, either file or directory.
   *        $data->type '';
   *        // The contents of this element, for files the content of the file for
   *        // directories a multidimmenstional array.
   *        $data->contents '';
   *     @endcode
   */
  public function getObject($repo_location, $path, $branch, $commit) {
  }

  /** 
   * Get a representation of an Object within the Git Repository.
   *
   * @param string $repo_location
   *   The path to the actual repository on disk.
   * @param string $branch
   *   The branch on which we are trying to view this object.
   *   TODO: This is here out of a vague suspicion that it'll be critical for svn
   *   (if this is abstracted that far).  Should it be?
   * @param string $commit
   *   The revision at which we are trying to view this object.
   *
   * @return string
   *   The contents of the commit.
   */
  public function getCommit($repo_location, $branch, $commit) {
  }

  /**
   * Send a 
   */
  private function sendRequest($method, $data) {
    $post_data['method'] = $method;
    $post_data['data'] = $data;
    $response_data = drupal_http_request($this->remoteURL, array('Content-Type' => 'application/x-www-form-urlencoded'), 'POST', http_build_query($post_data));
    if (!$data = $response_data->data) {
      throw new Exception('Invalid data received from remote.');
    }
    return $response_data->result;
  }
}

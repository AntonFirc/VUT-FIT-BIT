<?php
/**
 * IPP test unit test.php
 * Anton Firc (xfirca00)
 */

define("ERR_BAD_PARAM", 10);
define("ERR_FILE", 12);

$tests =  0;    #count of carried out test
$passed = 0;    #count of tests passed

function run_test ($file, $parse_file, $int_file, $html_file) { #runs test from file
    $GLOBALS['tests'] += 1; #carried out tests ++
    $subfile = scandir($file);  #scan test folder content
    $arr = explode(".", $subfile[2]); # subfile[0] => . [1] => .. ; gets test files name
    shell_exec("php5.6 $parse_file < \"$file/$arr[0].src\" > \"$file/out.xml\"");  #run parse.php
    shell_exec("python3.6 $int_file --source=\"$file/out.xml\" > \"$file/result\"");   #run interpret.py with parse.php output
    if (shell_exec("echo $?") != shell_exec("cat \"$file/$arr[0].rc\"")) {  #if exit code does not match
        fwrite($html_file, "<p>$file: <font color='red'>FAIL (exit code)</font></p>\n");
        $output = shell_exec("rm \"$file/out.xml\" \"$file/result\"");  #remove temp files
        print ("$output\n");
        return;
    }
    $output = shell_exec("diff \"$file/result\" \"$file/$arr[0].out\" >/dev/null ; echo $?");   # check interpret output
    if ($output == 0) { #if matches
        fwrite($html_file,"<p>$file: <font color='green'>PASS</font></p>\n");
        $GLOBALS['passed'] += 1;
    }
    else {  #does not match
        fwrite($html_file,"<p>$file: <font color='red'>FAIL (output)</font></p>\n");
    }
    $output = shell_exec("rm \"$file/out.xml\" \"$file/result\"");  #remove tmp files
    print ("$output\n");
}

function open_subdirs($path, $parse_file, $int_file, $recursion, $file) {  #recursively opens subdirectories with tests
    $dir = scandir($path);  #scan directory contents
    foreach($dir as $folder) {
        if ($folder == "." OR $folder == ".." OR $folder[0] == '.') continue;   #skip .; ..; and .name files
        if (is_dir($path . "/" . $folder)) {   #if directory found
            $path_n = $path . "/$folder";   #store path to directory
            if (is_test($path_n)) { #directory contains test files
                run_test($path_n, $parse_file, $int_file, $file);
            }
            else {  #directory does not contain test files
                if ($recursion == true) {
                    open_subdirs($path_n, $parse_file, $int_file, $recursion, $file);   #search for subdirectories
                }
            }
        }
    }
}


function is_test($folder) { #checks if folder contains test files with regular expressions
    $folderd = scandir($folder);    #scan content
    /* variables for each mandatory test file */
    $src = false;
    $out = false;
    $in = false;
    $rc = false;
    foreach ($folderd as $file) {
        if (preg_match('/.*\.src$/', $file)) {
            $src = true;
        }
        if (preg_match('/.*\.out$/', $file)) {
            $out = true;
        }
        if (preg_match('/.*\.in$/', $file)) {
            $in = true;
        }
        if (preg_match('/.*\.rc$/', $file)) {
            $rc = true;
        }
    }
    if ($src == true AND $out == true AND $in == true AND $rc == true) {    #if all test files found
        return true;
    }
    else {  #not all test files found
        return false;
    }
}

$shortopts = "";
$longopts = array ("help", "directory:", "recursive", "parse-script:", "int-script:");
$help_text = "###          IPP 2018 | test.php         ###\nTest unit for parse.php and interpret.py\n";

$options = getopt($shortopts, $longopts);

$test_dir = getcwd(); #directory with test files (current work directory)
$recursive = false; #recursion - script argument
$parse_file;    #parse.php file location
$int_file;  #interpret.py file location
$c_path; #current path to folder

if (isset($options["help"])) {  # if --help argument used
    if ($argc <= 2) {          # only --help can be used at one time
        echo "$help_text";
        exit(0);                # everything executed correctly
    }
    else {
        fwrite(STDERR, "ERROR: too many arguments !");
        exit(ERR_BAD_PARAM);
    }
}
if ($argc != 3 AND $argc != 4 AND $argc != 5 ){    #if run with wrong argument count
    fwrite(STDERR, "ERROR: wrong argument count !");
    exit(ERR_BAD_PARAM);
}
if (isset($options["directory"])) { #set test files directory
    $test_dir = $options["directory"];
}
if (isset($options["recursive"])) { #recursion
    $recursive = true;
}

if (isset($options["parse-script"])) {  #parse-script="file"
    $parse_file = $options["parse-script"];
}
else {  #mandatory argument not set
    fwrite(STDERR, "ERROR: too many arguments !");
    exit(ERR_BAD_PARAM);
}
if (isset($options["parse-script"])) {  #int-script="file"
    $int_file = $options["int-script"];
}
else {  #mandatory argument not set
    fwrite(STDERR, "ERROR: too many arguments !");
    exit(ERR_BAD_PARAM);
}

/* open output html file and write first lines */
$file = fopen("index.html", "w");
if ($file == false) {
    fwrite(STDERR, "ERROR: cannot open index.html !");
    exit(ERR_FILE);
}
fwrite($file, "<!DOCTYPE html>\n");
fwrite($file, "<html>\n");
fwrite($file, "<body>\n");
fwrite($file, "<h2>Tests:</h2>");

open_subdirs($test_dir, $parse_file, $int_file, $recursive, $file); #start browsing test directories

/* calculate test pass rate in % */
$pass_rate = $passed / $tests;
$pass_rate *= 100;
/* set color according to pass rate % */
if ($pass_rate > 80) {
    $color = 'green';
}
elseif ($pass_rate > 40) {
    $color = 'orange';
}
else {
    $color = 'red';
}
fwrite($file, "<h3>Test summary:\n</h3>");
fwrite($file, "<p>$passed of $tests tests passed => <font color=$color>$pass_rate%</font></p>\n");  #print test summary
/* ending html tags and close file */
fwrite($file, "</html>\n");
fwrite($file, "</body>\n");
if (fclose($file) == false) {
    fwrite(STDERR, "ERROR: cannot close index.html !");
    exit(ERR_FILE);
}
return 0;



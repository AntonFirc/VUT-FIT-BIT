<?php
/**
 * Created by PhpStorm.
 * User: afirc
 * Date: 2/12/18
 * Time: 8:56 AM
 */

include 'definitions.php';

$shortopts = "";
$longopts = array ("help", "comments", "loc", "stat:");
$help_text = "###          IPP 2018 | parse.php         ###\nLexical and syntactical analysis for IPCode18\nThis script reads input from STDIN\nNo arguments required !\n[--help] => show this dialogue\n";

$options = getopt($shortopts, $longopts);

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
elseif ($argc != 1 AND $argc != 3 AND $argc != 4){    #if run with arguments other than --help
    fwrite(STDERR, "ERROR: wrong argument count !");
    exit(ERR_BAD_PARAM);
}

$do_loc = false;    #show loc statistic ?
$do_comment = false;    #show commentary statistic ?

if (isset($options["loc"])) {
    if (isset($options["stat"])) {
        $do_loc = true;             #if --loc set correctly show loc statistic
    }
    else {
        fwrite(STDERR, "ERROR: --loc must be used with --stat=\"filename\" argument!");
        exit(ERR_BAD_PARAM);
    }
}
if (isset($options["comments"])) {
    if (isset($options["stat"])) {
        $do_comment = true;         #if --comments set correctly show commentary statistic
    }
    else {
        fwrite(STDERR, "ERROR: --loc must be used with --stat=\"filename\" argument!");
        exit(ERR_BAD_PARAM);
    }
}

$comment = 0;   #counter of lines with comments

$comment = parse_header($comment); #checks for header in soruce file
if (isset($options["stat"])) {  #if set statistics file send as argument
    parse_file($comment, $do_loc, $do_comment, $options["stat"]);
}
else {      #no statistics file
    parse_file($comment, false, false, NULL);
}

/*
 * Checks for .IPPCode18 header in source file
 */
function parse_header($comment) {
    $line = trim(fgets(STDIN), " \t\r\0\x0B"); #retrieve one line from STDIN sripped of blank chars on beginning and end
    while (strcmp($line, "\n") == 0 OR preg_match(comment_regex, $line)) {  #if commentary found increase commentary counter
        if (preg_match(comment_regex, $line)) {
            $comment++;
        }
        $line = trim(fgets(STDIN), " \t\r\0\x0B"); #retrieve one line from STDIN sripped of blank chars on beginning and end
    }
    $line = trim($line);    #remove \n character if present
    if (strcmp($line, header) != 0) {   #if header text not found in first line of text
        $maybe_comment = explode("#", $line);
        $line = trim($maybe_comment[0]);
        if (strcmp($line, header) == 0) {
            $comment++;
            return comment;
        }
        fwrite(STDERR, "ERROR: Missing header !");
        exit(ERR_LEXICAL_SYNTACTICAL);
    }
    return $comment; #returns count of lines with commentary before header
}

/*
 * Checks source file line by line
 */
function parse_file($comment, $do_loc, $do_comment, $statfile)
{
    #setup output XML file
    $xml = createXML();
    $xml_prog = $xml->createElement("program");
    $xml_prog->setAttribute("language", "IPPcode18");
    $inst_count = 1;
    $loc = 0;

    while (true) {

        $line = trim(fgets(STDIN), " \t\r\0\x0B"); #retrieve one line from STDIN sripped of blank chars on beginning and end
        if (strcmp($line, "\n") == 0) { #if empty line found skips it
            continue;
        }
        $line = trim($line);
        $split = explode(' ', $line);   #splits line into separate strings
        $found = false;
        if (my_isset(inst_keywords[$split[0]]) ) { #if found matching keyword
            $found = true;
        }
        if ($found == true) {   #if found matching keyword
            $arg_cnt = inst_keywords[$split[0]];    #number of arguments for given instruction
            if (my_isset($split[$arg_cnt + 1])) {      #if there is something behind last argument
                if (preg_match(comment_regex, $split[$arg_cnt + 1])) {
                    $comment++;
                }
                else {
                    fwrite(STDERR, "ERROR: Too many arguments for instruction \"$split[0]\"!\n");
                    exit(ERR_LEXICAL_SYNTACTICAL);
                }
            }
            for ($i = 1; $i <= $arg_cnt; $i++) {    #loop trouch parameters to check syntax
                if (empty($split[$i])) {   #if mandatory argument is missing
                    fwrite(STDERR, "ERROR: Too less arguments for instruction \"$split[0]\"!\n");
                    exit(ERR_LEXICAL_SYNTACTICAL);
                }
            }
            if ($arg_cnt == 0) {
                $loc++;
                #write instruction to XML
                $inst = $xml->createElement("instruction");
                $inst->setAttribute("order", $inst_count++);
                $inst->setAttribute("opcode", $split[0]);
                $xml_prog->appendChild( $inst );
            }
        }
        else {  #no matching instruction keyword found
            if (strlen($split[0]) == 0) {  #if reached end of file
                break;
            }
            else {
                $is_comment = explode('#', $split[0]);   #splits line into separate strings (removes comment)
                $tmp_03 = inst_keywords[$is_comment[0]];
                if (isset($tmp_03)) { #if found matching keyword
                    $comment++;
                    $arg_cnt = inst_keywords[$is_comment[0]];    #number of arguments for given instruction
                    if ($arg_cnt == 0) {
                        $loc++;
                        #writes instruction to XML
                        $inst = $xml->createElement("instruction");
                        $inst->setAttribute("order", $inst_count++);
                        $inst->setAttribute("opcode", $is_comment[0]);
                        $xml_prog->appendChild( $inst );
                    }
                }
                else {
                    fwrite(STDERR, "ERROR: Unknown instruction name: \"$split[0]\"!\n");
                    printf("%d\n", $split[0]);
                    exit(ERR_LEXICAL_SYNTACTICAL);
                }
            }
        }
        #instructions : DEFVAR/POPS <var>
        if (strcmp($split[0], "DEFVAR") == 0 OR strcmp($split[0], "POPS") == 0) {
            if (preg_match(var_regex, $split[1])) { #checks if argument matches var form
                $loc++;
                #write instruction to XML
                $inst = $xml->createElement("instruction");
                $inst->setAttribute("order", $inst_count++);
                $inst->setAttribute("opcode", $split[0]);
                $arg_1 = $xml->createElement("arg1");
                $arg_1->setAttribute("type", "var");
                $arg_1->textContent = $split[1];
                $inst->appendChild($arg_1);
                $xml_prog->appendChild( $inst );
            }
            else {
                fwrite(STDERR, "ERROR: Bad argument format: \"$split[1]\"!\n");
                exit(ERR_LEXICAL_SYNTACTICAL);
            }
        }
        #instructions MOVE/INT2CHAR/STRLEN/TYPE <var> <symb>
        if (strcmp($split[0], "MOVE") == 0 OR strcmp($split[0], "INT2CHAR") == 0 OR
            strcmp($split[0], "STRLEN") == 0 OR strcmp($split[0], "TYPE") == 0) {
            $loc++;
            #write instruction to XML
            $inst = $xml->createElement("instruction");
            $inst->setAttribute("order", $inst_count++);
            $inst->setAttribute("opcode", $split[0]);
            for ($i = 1; $i <= 2; $i++) {   # <symb> == variable
                if (preg_match(var_regex, $split[$i])) {
                    #add arguments to instruction element
                    $arg_1 = $xml->createElement("arg1");
                    $arg_1->setAttribute("type", "var");
                    $arg_1->textContent = $split[1];
                    $inst->appendChild($arg_1);
                    $xml_prog->appendChild( $inst );
                }
                else {      #<symb> = constant
                    if ($i == 2 && preg_match(const_regex, $split[$i])) {
                        #add arguments to instruction element
                        $constant = explode("@", $split[2]);
                        $arg_2 = $xml->createElement("arg2");
                        $arg_2->setAttribute("type", $constant[0]);
                        if (strcmp($constant[0], "bool") == 0) {    #convert TRUE|FALSE to lowercase
                            $constant[1] = strtolower($constant[1]);
                        }
                        $arg_2->textContent = $constant[1];
                        $inst->appendChild($arg_2);
                        $xml_prog->appendChild( $inst );
                    }
                    else {
                        fwrite(STDERR, "ERROR: Bad argument format: \"$split[$i]\"!\n");
                        exit(ERR_LEXICAL_SYNTACTICAL);
                    }
                }
            }
        }
        #instructions PUSHS/DPRINT/WRITE <symb>
        if (strcmp($split[0], "PUSHS") == 0 OR strcmp($split[0], "WRITE") == 0 OR
            strcmp($split[0], "DPRINT") == 0) {
            $loc++;
            #write instruction to XML
            $inst = $xml->createElement("instruction");
            $inst->setAttribute("order", $inst_count++);
            $inst->setAttribute("opcode", $split[0]);
            if (preg_match(const_regex, $split[1])) { #<symb> = constant
                #add arguments to instruction element
                $constant = explode("@", $split[1]); #split constant to (bool) (false)...
                $arg_1 = $xml->createElement("arg1");
                $arg_1->setAttribute("type", $constant[0]);
                if (strcmp($constant[0], "bool") == 0) {    #convert TRUE|FALSE to lowercase
                    $constant[1] = strtolower($constant[1]);
                }
                $arg_1->textContent = $constant[1];
                $inst->appendChild($arg_1);
                $xml_prog->appendChild( $inst );
            }
            else if (preg_match(var_regex, $split[1])) {    #<symb> = variable
                #add arguments to instruction element
                $arg_1 = $xml->createElement("arg1");
                $arg_1->setAttribute("type", "var");
                $arg_1->textContent = $split[1];
                $inst->appendChild($arg_1);
                $xml_prog->appendChild( $inst );
            }
            else {
                fwrite(STDERR, "ERROR: Bad argument format: \"$split[1]\"!\n");
                exit(ERR_LEXICAL_SYNTACTICAL);
            }
        }
        #instructions INSTR <var> <symb1> <symb2>
        if (my_isset(tri_arg_instr[$split[0]])) {
            $loc++;
            #write instruction to XML
            $inst = $xml->createElement("instruction");
            $inst->setAttribute("order", $inst_count++);
            $inst->setAttribute("opcode", $split[0]);
            if (preg_match(var_regex, $split[1])) {
                #add arguments to instruction element
                $arg_1 = $xml->createElement("arg1");
                $arg_1->setAttribute("type", "var");
                $arg_1->textContent = $split[1];
                $inst->appendChild($arg_1);
                $xml_prog->appendChild( $inst );
            }
            else {
                fwrite(STDERR, "ERROR: Bad argument format: \"$split[1]\"!\n");
                exit(ERR_LEXICAL_SYNTACTICAL);
            }
            for ($i = 2; $i <= 3; $i++) {
                if (preg_match(const_regex, $split[$i])) {
                    #add arguments to instruction element
                    $constant = explode("@", $split[$i]);  #split constant to (bool) (false)...
                    $arg_2 = $xml->createElement("arg$i");
                    $arg_2->setAttribute("type", $constant[0]);
                    if (strcmp($constant[0], "bool") == 0) {    #convert TRUE|FALSE to lowercase
                        $constant[1] = strtolower($constant[1]);
                    }
                    $arg_2->textContent = $constant[1];
                    $inst->appendChild($arg_2);
                    $xml_prog->appendChild( $inst );
                }
                else {
                    fwrite(STDERR, "ERROR: Bad argument format: \"$split[$i]\"!\n");
                    exit(ERR_LEXICAL_SYNTACTICAL);
                }
            }
        }
        #instructions LABEL/CALL/JUMP <label>
        if (strcmp($split[0], "LABEL") == 0 OR strcmp($split[0], "JUMP") == 0 OR
            strcmp($split[0], "CALL") == 0) {
            if (!preg_match(var_regex, $split[1]) AND !preg_match(const_regex, $split[1])) { #checks if is not variable or constant
                $loc++;
                #write instruction to XML
                $inst = $xml->createElement("instruction");
                $inst->setAttribute("order", $inst_count++);
                $inst->setAttribute("opcode", $split[0]);
                $arg_1 = $xml->createElement("arg1");
                $arg_1->setAttribute("type", "label");
                $arg_1->textContent = $split[1];
                $inst->appendChild($arg_1);
                $xml_prog->appendChild( $inst );
            }
            else {
                fwrite(STDERR, "ERROR: Bad argument format: \"$split[1]\"!\n");
                exit(ERR_LEXICAL_SYNTACTICAL);
            }
        }
        #instruction READ <var> <>
        if (strcmp($split[0], "READ") == 0) {
            if (preg_match(var_regex, $split[1])) { #if is var
                if (preg_match(type_regex, $split[2])) { #if is vartype
                    $loc++;
                    #write instruction to XML with arguments
                    $inst = $xml->createElement("instruction");
                    $inst->setAttribute("order", $inst_count++);
                    $inst->setAttribute("opcode", $split[0]);
                    $arg_1 = $xml->createElement("arg1");
                    $arg_1->setAttribute("type", "label");
                    $arg_1->textContent = $split[1];
                    $inst->appendChild($arg_1);
                    $arg_2 = $xml->createElement("arg2");
                    $arg_2->setAttribute("type", "type");
                    $arg_2->textContent = $split[2];
                    $inst->appendChild($arg_2);
                    $xml_prog->appendChild( $inst );
                }
                else {
                    fwrite(STDERR, "ERROR: Bad argument format: \"$split[2]\"!\n");
                    exit(ERR_LEXICAL_SYNTACTICAL);
                }
            }
            else {
                fwrite(STDERR, "ERROR: Bad argument format: \"$split[1]\"!\n");
                exit(ERR_LEXICAL_SYNTACTICAL);
            }
        }
        #instructions JUMPIFEQ/JUMPIFNEQ <label> <symb1> <symb2>
        if (strcmp($split[0], "JUMPIFEQ") == 0 OR strcmp($split[0], "JUMPIFNEQ") == 0) {
            $loc++;
            #write instruction to XML
            $inst = $xml->createElement("instruction");
            $inst->setAttribute("order", $inst_count++);
            $inst->setAttribute("opcode", $split[0]);
            if (!preg_match(var_regex, $split[1]) AND !preg_match(const_regex, $split[1])) {
                $arg_1 = $xml->createElement("arg1");
                $arg_1->setAttribute("type", "label");
                $arg_1->textContent = $split[1];
                $inst->appendChild($arg_1);
                $xml_prog->appendChild( $inst );
            }
            else {
                fwrite(STDERR, "ERROR: Bad argument format: \"$split[1]\"!\n");
                exit(ERR_LEXICAL_SYNTACTICAL);
            }
            for ($i = 2; $i <= 3; $i++) {
                if (preg_match(var_regex, $split[$i])) { # if <symb> = variable
                    #add arguments to instruction element
                    $arg_2 = $xml->createElement("arg$i");
                    $arg_2->setAttribute("type", "var");
                    $arg_2->textContent = $split[$i];
                    $inst->appendChild($arg_2);
                    $xml_prog->appendChild( $inst );
                }
                else if (preg_match(const_regex, $split[$i])) { #if <symb> = constant
                    #add arguments to instruction element
                    $constant = explode("@", $split[$i]);
                    $arg_2 = $xml->createElement("arg$i");
                    $arg_2->setAttribute("type", $constant[0]);
                    if (strcmp($constant[0], "bool") == 0) {        #convert TRUE|FALSE to lowercase
                        $constant[1] = strtolower($constant[1]);
                    }
                    $arg_2->textContent = $constant[1];
                    $inst->appendChild($arg_2);
                    $xml_prog->appendChild( $inst );
                }
                else {
                    fwrite(STDERR, "ERROR: Bad argument format: \"$split[$i]\"!\n");
                    exit(ERR_LEXICAL_SYNTACTICAL);
                }
            }
        }
    }

    $xml->appendChild( $xml_prog); #append XML element 'program' to XML file
    echo $xml->saveXML();
    if ($do_loc == true OR $do_comment == true) { #if set --loc or --comments
        $stats = fopen("$statfile", "w");
        if ($do_loc == true) {
            fwrite($stats, "$loc"); #write number of loc
            if ($do_comment == true) {
                fwrite($stats, "\n");   #write new line if comments stats are following
            }
        }
        if ($do_comment == true) {
            fwrite($stats, $comment);   #write number of lines with comments
        }
        fclose($stats); #close file
    }
    exit(0);    #successfull end
}

/*
 * opens new XML document
 */
function createXML() {
    $xmlDoc = new DOMDocument('1.0', 'UTF-8'); #opens file and specifies header
    #formatting of XML file
    $xmlDoc->preserveWhiteSpace = false;
    $xmlDoc->formatOutput = true;
    return $xmlDoc; #return hande to file
}

function my_isset($input) {
    return isset($input);
}


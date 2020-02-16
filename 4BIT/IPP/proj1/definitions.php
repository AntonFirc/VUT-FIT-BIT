<?php
/**
 * Created by PhpStorm.
 * User: afirc
 * Date: 2/12/18
 * Time: 1:31 PM
 */

/*
 * Error codes
 */
define("ERR_BAD_PARAM", 10);
define("ERR_INPUT_OPEN", 11);
define("ERR_OUTPUT_OPEN", 12);
define("ERR_LEXICAL_SYNTACTICAL", 21);
define("ERR_INTERNAL", 99);

/*
 *  Header must be included in IPPcode18 file
 */
const header = ".IPPcode18";

/*
 * Instructions
 */
const inst_keywords = array("MOVE" => 2, "CREATEFRAME" => 0, "PUSHFRAME" => 0, "POPFRAME" => 0, "DEFVAR" => 1,
                            "CALL" => 1, "RETURN" => 0, "PUSHS" => 1, "POPS" => 1, "ADD" => 3, "SUB" => 3, "MUL" => 3,
                            "IDIV" => 3, "LT" => 3, "GT" => 3, "EQ" => 3, "AND" => 3, "OR" => 3, "NOT" => 3,
                            "INT2CHAR" => 2, "STR2INT" => 3, "READ" => 2, "WRITE" => 1, "CONCAT" => 3, "STRLEN" => 2,
                            "GETCHAR" => 3, "SETCHAR" => 3, "TYPE" => 2, "LABEL" => 1, "JUMP" => 1, "JUMPIFEQ" => 3,
                            "JUMPIFNEQ" => 3, "DPRINT" => 1, "BREAK" => 0);

const tri_arg_instr = array("ADD" => 0, "SUB" => 0, "MUL" => 0, "IDIV" => 0, "LT" => 0, "GT" => 0, "EQ" => 0, "AND" => 0,
                          "OR" => 0, "NOT" => 0, "CONCAT" => 0, "GETCHAR" => 0, "SETCHAR" => 0);

const var_regex = '/[GLT]F@.*$/';
const const_regex = '/(string@(.*)$|int@\d+$|bool@(true|false|TRUE|FALSE)$)/';
const type_regex = '/(int|string|bool)/';
const comment_regex = '/^#.*/';
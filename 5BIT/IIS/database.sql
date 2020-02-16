SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


-- --------------------------------------------------------

--
-- Štruktúra tabuľky pre tabuľku `FILM`
--

CREATE TABLE IF NOT EXISTS `FILM` (
  `NAZEV` varchar(20) COLLATE latin2_czech_cs DEFAULT NULL,
  `ZANR` varchar(20) COLLATE latin2_czech_cs DEFAULT NULL,
  `POPIS` varchar(500) COLLATE latin2_czech_cs DEFAULT NULL,
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `CENA_DOSPELY` int(11) DEFAULT NULL,
  `CENA_ZLAVNENY` int(11) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs AUTO_INCREMENT=36 ;

--
-- Sťahujem dáta pre tabuľku `FILM`
--

INSERT INTO `FILM` (`NAZEV`, `ZANR`, `POPIS`, `ID`, `CENA_DOSPELY`, `CENA_ZLAVNENY`) VALUES


-- --------------------------------------------------------

--
-- Štruktúra tabuľky pre tabuľku `KINO`
--

CREATE TABLE IF NOT EXISTS `KINO` (
  `LOKACE` varchar(100) COLLATE latin2_czech_cs DEFAULT NULL,
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs AUTO_INCREMENT=3 ;

--
-- Sťahujem dáta pre tabuľku `KINO`
--

INSERT INTO `KINO` (`LOKACE`, `ID`) VALUES


-- --------------------------------------------------------

--
-- Štruktúra tabuľky pre tabuľku `PROMITACI_SAL`
--

CREATE TABLE IF NOT EXISTS `PROMITACI_SAL` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `KINO_ID` int(11) DEFAULT NULL,
  `CISLO_SALU` int(20) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs AUTO_INCREMENT=5 ;

--
-- Sťahujem dáta pre tabuľku `PROMITACI_SAL`
--

INSERT INTO `PROMITACI_SAL` (`ID`, `KINO_ID`, `CISLO_SALU`) VALUES


-- --------------------------------------------------------

--
-- Štruktúra tabuľky pre tabuľku `PROMITACI_SAL_PROGRAM`
--

CREATE TABLE IF NOT EXISTS `PROMITACI_SAL_PROGRAM` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `CAS` datetime DEFAULT NULL,
  `FILM_ID` int(11) DEFAULT NULL,
  `SALA_ID` int(20) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs AUTO_INCREMENT=11 ;

--
-- Sťahujem dáta pre tabuľku `PROMITACI_SAL_PROGRAM`
--

INSERT INTO `PROMITACI_SAL_PROGRAM` (`ID`, `CAS`, `FILM_ID`, `SALA_ID`) VALUES


-- --------------------------------------------------------

--
-- Štruktúra tabuľky pre tabuľku `REZERVACE`
--

CREATE TABLE IF NOT EXISTS `REZERVACE` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `ID_PROMITANI` int(11) NOT NULL,
  `RAD` varchar(1) COLLATE latin2_czech_cs NOT NULL,
  `SEDADLO` int(11) NOT NULL,
  `UZIVATEL_ID` int(11) NOT NULL,
  `ZAPLACENO` int(1) DEFAULT '0',
  `ZLAVNENY` int(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs AUTO_INCREMENT=90 ;

--
-- Sťahujem dáta pre tabuľku `REZERVACE`
--

INSERT INTO `REZERVACE` (`ID`, `ID_PROMITANI`, `RAD`, `SEDADLO`, `UZIVATEL_ID`, `ZAPLACENO`, `ZLAVNENY`) VALUES


-- --------------------------------------------------------

--
-- Štruktúra tabuľky pre tabuľku `UZIVATEL`
--

CREATE TABLE IF NOT EXISTS `UZIVATEL` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `JMENO` varchar(20) CHARACTER SET utf8 COLLATE utf8_czech_ci DEFAULT NULL,
  `PRIJMENI` varchar(20) CHARACTER SET utf8 COLLATE utf8_czech_ci DEFAULT NULL,
  `PRACOVNI_POZICE` varchar(20) COLLATE latin2_czech_cs DEFAULT NULL,
  `LOGIN` varchar(50) COLLATE latin2_czech_cs DEFAULT NULL,
  `HESLO` varchar(100) COLLATE latin2_czech_cs DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs AUTO_INCREMENT=21 ;

--
-- Sťahujem dáta pre tabuľku `UZIVATEL`
--

INSERT INTO `UZIVATEL` (`ID`, `JMENO`, `PRIJMENI`, `PRACOVNI_POZICE`, `LOGIN`, `HESLO`) VALUES
(1, 'Chuck', 'Norris', 'admin', 'admin', '$2y$10$mWBklNrVtqMVJjxe7mXh0u2IGv6YzbtDd/kylJEZJdfl3FJ1Lbduy'),

--
-- Spúšťače `UZIVATEL`
--
DROP TRIGGER IF EXISTS `usr_delete`;
DELIMITER //
CREATE TRIGGER `usr_delete` AFTER DELETE ON `UZIVATEL`
 FOR EACH ROW DELETE FROM REZERVACE WHERE REZERVACE.UZIVATEL_ID = old.ID
//
DELIMITER ;


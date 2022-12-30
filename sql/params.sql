.separator "\t"

CREATE TABLE bands(
    id          INTEGER PRIMARY KEY,
    name        TEXT,
    start_freq  INTEGER,
    stop_freq   INTEGER,
    ham         INTEGER
);

.import bands_r1.csv bands

CREATE TABLE band_params(
    bands_id    INTEGER,
    name        TEXT,
    val         TEXT,
    UNIQUE      (bands_id, name) ON CONFLICT REPLACE
);

.import band_params.csv band_params

CREATE TABLE params(
    name        TEXT PRIMARY KEY ON CONFLICT REPLACE,
    val         TEXT,
    val_def     TEXT
);

.import params.csv params

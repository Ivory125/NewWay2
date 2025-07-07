ALTER TABLE `account`
ADD COLUMN `language` tinyint(4) UNSIGNED NOT NULL DEFAULT 1 AFTER `last_play`;
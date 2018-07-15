--[[
-- database struct

-- 1.
-- Current Database: `testdb`
--
-- SELECT * FROM information_schema.SCHEMATA where SCHEMA_NAME='testdb';
drop database if exists `testdb`;
CREATE DATABASE /*!32312 IF NOT EXISTS*/ `testdb` /*!40100 DEFAULT CHARACTER SET utf8 */;

-- 2.
use `testdb`;
SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for accountsinfo
-- ----------------------------
DROP TABLE IF EXISTS `accountsinfo`;
CREATE TABLE `accountsinfo` (
  `UserID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `GameID` int(10) unsigned NOT NULL DEFAULT '0',
  `NickName` char(32) DEFAULT NULL,
  `Gender` tinyint(4) NOT NULL DEFAULT '0' COMMENT '用户性别',
  PRIMARY KEY (`UserID`),
  KEY `idx1` (`GameID`),
  KEY `idx2` (`NickName`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;

-- 3.
INSERT INTO `accountsinfo` VALUES (1003, 56789, 'liace', 1);
INSERT INTO `accountsinfo` VALUES (1004, 56779, 'tomer', 0);

-- 4.
-- ----------------------------
-- Procedure structure for sp_query_user
-- ----------------------------
DROP PROCEDURE IF EXISTS `sp_query_user`;
DELIMITER ;;
CREATE  PROCEDURE `sp_query_user`(IN inUserID int(11))
    SQL SECURITY INVOKER
THIS_PROCEDURE:BEGIN
	DECLARE retCode INT;
	DECLARE retMsg VARCHAR(255);

	-- 基本信息
	DECLARE varUserID INT;
	DECLARE varGameID INT;
	DECLARE varGender TINYINT;
    DECLARE varNickName CHAR(32);
	-- 查询用户
	SELECT `UserID`, `NickName`, `GameID`, `Gender` FROM `accountsinfo` WHERE `UserID`=inUserID;

	IF ISNULL(varUserID) THEN
		SET retCode := 4;
		SET retMsg = "您的帐号不存在";
		SELECT retCode, retMsg;
		LEAVE THIS_PROCEDURE;
    END IF;
    
	SELECT retCode, retMsg,  varUserID AS "UserID", varGameID AS "GameID", varGender AS "Gender", varNickName AS "NickName";
END
;;
DELIMITER ;


]]






local luasql = require "luasql.mysql"
local printr = require "print_r"

local _env, _conn

local mysqlInfo = {
	mysqlHost = "127.0.0.1",
	mysqlUser = "root",
	mysqlPassword = "game123456",
	mysqlDataBase = "testdb",
}

local function initFunc()
    _env = luasql.mysql()
    _conn = assert(_env:connect(
        mysqlInfo.mysqlDataBase,
        mysqlInfo.mysqlUser,
        mysqlInfo.mysqlPassword,
        mysqlInfo.mysqlHost
    ))
    _conn:execute("set names 'utf8'")
end


-- this function never throw
local function disconnect()
    if _conn ~= nil then
        _conn:close()
         _conn = nil
    end
    if _env ~= nil then
        _env:close()
        _env = nil
    end
end




local function checkReturn(ret, errmsg)
	if ret==nil then
		--skynet.send(addressResolver.getAddressByServiceName("mysqlConnectionPool"), "lua", "connectionError", skynet.self())
		error(errmsg)
	end
end

local function cmd_exit()
	disconnect()
	--skynet.exit()
end


local function cmd_execute(sql)
	local ret, errmsg = _conn:execute(sql)
	checkReturn(ret, errmsg)
end


local function cmd_query(sql)
	local cursor, errmsg = _conn:execute(sql)
	checkReturn(cursor, errmsg)
	
	if type(cursor)=='number' then
		return cursor
	end

	local rows = {}
	while true do
		local row = cursor:fetch({}, 'a')
		if row then
			table.insert(rows, row)
		else
			break;
		end
	end
	cursor:close()
	return rows
end

local function cmd_insert(sql)
	local cursor, errmsg = _conn:execute(sql)
	checkReturn(cursor, errmsg)
	if type(cursor)~='number' then
		return nil
	end

	return _conn:getlastautoid()
end

local function cmd_call(sql)
    print("cmd_call - ",sql)
    local cursor, errmsg = _conn:callprocedure(sql)
	checkReturn(cursor, errmsg)	
	local rows = {}
	while true do
		local row = cursor:fetch({}, 'a')
		if row then
			table.insert(rows, row)
		else
			break;
		end
	end
	cursor:close()
	return rows
end


initFunc();
print("initFunc - _env, _conn",_env, _conn)

local sql = "INSERT INTO `testdb`.`accountsinfo` VALUES (1006, 56777, 'teony', 2);"
cmd_execute(sql)


local sql = "SELECT * FROM `testdb`.`accountsinfo`;"
local rows = cmd_query(sql)
print("cmd_query - accountsinfo - 1")
printr(rows)


local sql = "DELETE FROM `testdb`.`accountsinfo` where userid = 1006;"
cmd_execute(sql)

local sql = "SELECT * FROM `testdb`.`accountsinfo`;"
local rows = cmd_query(sql)
print("cmd_query - accountsinfo - 2")
printr(rows)


local sql = string.format("call testdb.sp_query_user(%d)", 1003)
local rows = cmd_call(sql)
--local row = rows[1]
--row.retCode = tonumber(row.retCode)
--local msg=row.retMsg
print("cmd_call - sp_query_user - 1")
printr(rows)

local sql = string.format("call testdb.sp_query_user(%d)", 1006)
local rows = cmd_call(sql)
--local row = rows[1]
--row.retCode = tonumber(row.retCode)
--local msg=row.retMsg
print("cmd_call - sp_query_user - 2")
printr(rows)

cmd_exit()
print("cmd_exit - _env, _conn",_env, _conn)




#include "mysql_mgr.h"
#include <iomanip>



bool CMysqlMgr::Init()
{
	m_dbConfig[DB_INDEX_TYPE_ACCOUNT].port = 3306;
	m_dbConfig[DB_INDEX_TYPE_ACCOUNT].timeout = 0;
	m_dbConfig[DB_INDEX_TYPE_ACCOUNT].host = "127.0.0.1";
	m_dbConfig[DB_INDEX_TYPE_ACCOUNT].user = "root";
	m_dbConfig[DB_INDEX_TYPE_ACCOUNT].password = "game123456";
	m_dbConfig[DB_INDEX_TYPE_ACCOUNT].database = "db_accounts";

	m_dbConfig[DB_INDEX_TYPE_RECORD].port = 3306;
	m_dbConfig[DB_INDEX_TYPE_RECORD].timeout = 0;
	m_dbConfig[DB_INDEX_TYPE_RECORD].host = "127.0.0.1";
	m_dbConfig[DB_INDEX_TYPE_RECORD].user = "root";
	m_dbConfig[DB_INDEX_TYPE_RECORD].password = "game123456";
	m_dbConfig[DB_INDEX_TYPE_RECORD].database = "db_record";

	m_dbConfig[DB_INDEX_TYPE_TREASURE].port = 3306;
	m_dbConfig[DB_INDEX_TYPE_TREASURE].timeout = 0;
	m_dbConfig[DB_INDEX_TYPE_TREASURE].host = "127.0.0.1";
	m_dbConfig[DB_INDEX_TYPE_TREASURE].user = "root";
	m_dbConfig[DB_INDEX_TYPE_TREASURE].password = "game123456";
	m_dbConfig[DB_INDEX_TYPE_TREASURE].database = "db_treasure";

	for (int i = 0; i < DB_INDEX_TYPE_MAX; i++)
	{
		struct tagDataBaseConfig & config = m_dbConfig[i];
		m_dbSyncOper[i].connect(config.host.data(), config.port, config.user.data(), config.password.data(), config.database.data(), config.timeout);
		if (m_dbSyncOper[i].connected() == false)
		{
			return false;
		}
	}
	m_lLastCheckTime = GetMillisecond();

	return true;
}

void CMysqlMgr::ShutDown()
{

}

unsigned long long	CMysqlMgr::GetMillisecond()
{
	timespec _spec;
	clock_gettime(CLOCK_MONOTONIC, &_spec);
	unsigned long long millisecond = _spec.tv_sec * 1000 + _spec.tv_nsec / 1000 / 1000;
	return millisecond;
}

void CMysqlMgr::OnCheckConnect()
{
	unsigned long long lCurrentTime = GetMillisecond();
	if (m_lLastCheckTime + CHECK_CONNECT_TIME > lCurrentTime)
	{
		for (int i = 0; i < DB_INDEX_TYPE_MAX; i++)
		{
			if (m_dbSyncOper[i].connected() == false)
			{
				m_dbSyncOper[i].reconnect();
				return;
			}
		}
		m_lLastCheckTime = lCurrentTime;
	}
}

void CMysqlMgr::OnMysqlTick()
{
	OnCheckConnect();

}
void CMysqlMgr::TestMysql()
{
	TestMysql_One();
	TestMysql_Two();
	TestMysql_Three();
}


void CMysqlMgr::TestMysql_One()
{
	return;
	try
	{
		db::mysql mysql;
		mysql.connect("127.0.0.1", 3306, "root", "game123456", "mysql");

		auto sql = R"(
            DROP TABLE IF EXISTS `article_detail`;
            CREATE TABLE `article_detail` (
            `id` BIGINT NOT NULL,
            `parentid` BIGINT DEFAULT '0',
            `title` varchar(255) DEFAULT '',
            `content` varchar(255) DEFAULT '',
            `updatetime` varchar(255) DEFAULT '',
            PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
        )";

		bool bconnected = mysql.connected();
		std::cout << "bconnected:" << bconnected << std::endl;

		mysql.execute(sql);
		mysql.execute("INSERT INTO article_detail(id,parentid,title,content,updatetime) VALUES(1,2,'abc','abc','abc')");

		auto stmtid = mysql.prepare("INSERT INTO article_detail(id,parentid,title,content,updatetime) VALUES(?,?,?,?,?)");
		//stmt
		mysql.execute_stmt(stmtid, 2, 2, "abc", "abc", "abc");

		auto result = mysql.query<db::data_table>("select * from article_detail;");
		for (auto& row : *result)
		{
			std::cout << std::get<int64_t>(row[0]) << std::endl;
			std::cout << std::get<int64_t>(row[1]) << std::endl;
			std::cout << std::get<std::string>(row[2]) << std::endl;
			std::cout << std::get<std::string>(row[3]) << std::endl;
			std::cout << std::get<std::string>(row[4]) << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cout << "mysql exception:" << e.what() << std::endl;
	}
}

void CMysqlMgr::TestMysql_Two()
{
	auto sql_accountsinfo = R"(
DROP TABLE IF EXISTS `accountsinfo`;
CREATE TABLE `accountsinfo` (
  `UserID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `GameID` int(10) unsigned NOT NULL DEFAULT '0',
  `PlatformID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '统一平台ID',
  `NickName` char(32) DEFAULT NULL,
  `Gender` tinyint(4) NOT NULL DEFAULT '0' COMMENT '用户性别',
  `FaceID` smallint(3) NOT NULL DEFAULT '1' COMMENT '头像标识',
  `Experience` int(11) NOT NULL DEFAULT '0' COMMENT '经验数值',
  `LoveLiness` int(11) NOT NULL DEFAULT '0' COMMENT '用户魅力',
  PRIMARY KEY (`UserID`),
  KEY `idx1` (`GameID`),
  KEY `idx2` (`NickName`),
  KEY `idx3` (`PlatformID`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;
)";
	auto sql_accountsface = R"(
DROP TABLE IF EXISTS `accountsface`;
CREATE TABLE `accountsface` (
  `UserID` int(10) unsigned NOT NULL,
  `PlatformFace` char(32) DEFAULT NULL,
  PRIMARY KEY (`UserID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
)";

	m_dbSyncOper[DB_INDEX_TYPE_ACCOUNT].execute(sql_accountsinfo);
	m_dbSyncOper[DB_INDEX_TYPE_ACCOUNT].execute(sql_accountsface);

	auto sql_gamescoreinfo = R"(
DROP TABLE IF EXISTS `gamescoreinfo`;
CREATE TABLE `gamescoreinfo` (
  `UserID` int(10) unsigned NOT NULL DEFAULT '0',
  `Score` bigint(20) NOT NULL DEFAULT '0' COMMENT '',
  `Revenue` bigint(20) NOT NULL DEFAULT '0' COMMENT '',
  `InsureScore` bigint(20) NOT NULL DEFAULT '0' COMMENT '',
  `WinCount` int(11) NOT NULL DEFAULT '0' COMMENT '',
  `LostCount` int(11) NOT NULL DEFAULT '0' COMMENT '',
  `FleeCount` int(11) NOT NULL DEFAULT '0' COMMENT '',
  `DrawCount` int(11) NOT NULL DEFAULT '0' COMMENT '',
  PRIMARY KEY (`UserID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
)";
	auto sql_gameproperty = R"(
DROP TABLE IF EXISTS `gameproperty`;
CREATE TABLE `gameproperty` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '道具标识',
  `Name` varchar(255) NOT NULL COMMENT '道具名字',
  `Gold` bigint(20) NOT NULL COMMENT '道具金币',
  `Discount` smallint(6) NOT NULL DEFAULT '90' COMMENT '会员折扣',
  `IssueArea` smallint(6) NOT NULL DEFAULT '3' COMMENT '发行范围',
  `ServiceArea` smallint(6) NOT NULL COMMENT '使用范围',
  `SendLoveLiness` bigint(20) NOT NULL COMMENT '增加魅力',
  `RecvLoveLiness` bigint(20) NOT NULL COMMENT '增加魅力',
  `RegulationsInfo` varchar(255) NOT NULL COMMENT '使用说明',
  `Nullity` tinyint(4) NOT NULL DEFAULT '0' COMMENT '禁止标志',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=212 DEFAULT CHARSET=utf8;
)";
	m_dbSyncOper[DB_INDEX_TYPE_TREASURE].execute(sql_gamescoreinfo);
	m_dbSyncOper[DB_INDEX_TYPE_TREASURE].execute(sql_gameproperty);


	auto sql_gameproperty_insert_value = R"(
INSERT INTO `gameproperty` VALUES ('1', '汽车', '1500', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('2', '臭蛋', '100', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('3', '鼓掌', '300', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('4', '香吻', '200', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('5', '啤酒', '500', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('6', '蛋糕', '800', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('7', '钻戒', '1000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('8', '暴打', '500', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('9', '炸弹', '1000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('10', '香烟', '500', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('11', '别墅', '2000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('12', '砖头', '300', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('13', '鲜花', '300', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('14', '双倍积分卡', '10000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('15', '四倍积分卡', '20000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('16', '负分清零卡', '20000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('17', '清逃跑率卡', '10000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('18', '小喇叭', '10000', '90', '7', '7', '0', '0', '', '0');
INSERT INTO `gameproperty` VALUES ('19', '大喇叭', '50000', '90', '7', '7', '0', '0', '', '0');
INSERT INTO `gameproperty` VALUES ('20', '防踢卡', '10000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('21', '护身符', '10000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('22', '蓝钻会员卡', '100000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('23', '蓝钻会员卡', '300000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('24', '白钻会员卡', '600000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('25', '红钻会员卡', '1200000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('26', 'VIP房卡', '1000000', '90', '7', '7', '0', '0', '', '1');
INSERT INTO `gameproperty` VALUES ('100', '鸡蛋', '500', '90', '7', '7', '0', '-1', '', '1');
INSERT INTO `gameproperty` VALUES ('101', '香蕉皮', '500', '90', '7', '7', '0', '-1', '', '1');
INSERT INTO `gameproperty` VALUES ('102', '西红柿', '500', '90', '7', '7', '0', '-1', '', '1');
INSERT INTO `gameproperty` VALUES ('103', '飞刀', '5000', '90', '7', '7', '0', '-10', '', '1');
INSERT INTO `gameproperty` VALUES ('104', '炸弹', '50000', '90', '7', '7', '0', '-100', '', '1');
INSERT INTO `gameproperty` VALUES ('105', '帽子', '500', '90', '7', '7', '0', '1', '', '1');
INSERT INTO `gameproperty` VALUES ('106', '玫瑰', '500', '90', '7', '7', '0', '1', '', '1');
INSERT INTO `gameproperty` VALUES ('107', '红唇', '500', '90', '7', '7', '0', '1', '', '1');
INSERT INTO `gameproperty` VALUES ('108', '啤酒', '5000', '90', '7', '7', '0', '10', '', '1');
INSERT INTO `gameproperty` VALUES ('109', '香槟', '5000', '90', '7', '7', '0', '10', '', '1');
INSERT INTO `gameproperty` VALUES ('110', '名表', '5000', '90', '7', '7', '0', '10', '', '1');
INSERT INTO `gameproperty` VALUES ('200', '水晶鞋', '100', '100', '7', '7', '0', '10', '', '1');
INSERT INTO `gameproperty` VALUES ('201', '戒指', '200', '100', '7', '7', '0', '20', '', '1');
INSERT INTO `gameproperty` VALUES ('202', '钻戒', '200', '100', '7', '7', '0', '20', '', '1');
INSERT INTO `gameproperty` VALUES ('203', '摇钱树', '500', '100', '7', '7', '0', '50', '', '1');
INSERT INTO `gameproperty` VALUES ('204', '摩托', '1000', '100', '7', '7', '0', '100', '', '1');
INSERT INTO `gameproperty` VALUES ('205', '汽车', '50000', '100', '7', '7', '0', '100', '', '1');
INSERT INTO `gameproperty` VALUES ('206', '飞机', '5000', '100', '7', '7', '0', '500', '', '1');
INSERT INTO `gameproperty` VALUES ('207', '游轮', '10000', '100', '7', '7', '0', '1000', '', '1');
INSERT INTO `gameproperty` VALUES ('208', '玫瑰', '1000', '90', '7', '7', '0', '1', '', '0');
INSERT INTO `gameproperty` VALUES ('209', '金砖', '10000', '90', '7', '7', '0', '10', ' ', '0');
INSERT INTO `gameproperty` VALUES ('210', '跑车', '100000', '90', '7', '7', '0', '100', ' ', '0');
INSERT INTO `gameproperty` VALUES ('211', '别墅', '1000000', '90', '7', '7', '0', '1000', ' ', '0');
)";
	m_dbSyncOper[DB_INDEX_TYPE_TREASURE].execute(sql_gameproperty_insert_value);

}

void CMysqlMgr::TestMysql_Three()
{
	std::shared_ptr<db::data_table> sptr_result = m_dbSyncOper[DB_INDEX_TYPE_TREASURE].query<db::data_table>("select * from gameproperty where ID=211 or ID=1;");
	std::cout << "gameproperty - row_size:" << sptr_result->row_size() << std::endl;
	std::cout << "gameproperty - column_size:" << sptr_result->column_size() << std::endl;
	if (sptr_result != nullptr)
	{
		int index_clos = 0;
		auto iter_vec_cols = sptr_result->get_cols();
		std::cout << "----------------------------------------------------------" << std::endl;
		for (auto iter_clo = iter_vec_cols.begin(); iter_clo != iter_vec_cols.end(); iter_clo++)
		{
			std::cout << index_clos++ << " - colname:" << std::setw(20) << std::setiosflags(std::ios::left) << std::setfill(' ') << iter_clo->first  << "  data_type:" << iter_clo->second << std::endl;
		}

		for (auto iter_rows = sptr_result->begin(); iter_rows != sptr_result->end(); iter_rows++)
		{
			auto & row = (*iter_rows);
			std::cout << "----------------------------------------------------------" << std::endl;
			std::cout << "0 - " << std::get<int>(row[0]) << std::endl;
			std::cout << "1 - " << std::get<std::string>(row[1]) << std::endl;
			std::cout << "2 - " << std::get<int64_t>(row[2]) << std::endl;
			std::cout << "3 - " << std::get<int>(row[3]) << std::endl;
			std::cout << "4 - " << std::get<int>(row[4]) << std::endl;
			std::cout << "5 - " << std::get<int>(row[5]) << std::endl;
			std::cout << "6 - " << std::get<int64_t>(row[6]) << std::endl;
			std::cout << "7 - " << std::get<int64_t>(row[7]) << std::endl;
			std::cout << "8 - " << std::get<std::string>(row[8]) << std::endl;
			std::cout << "9 - " << std::get<int>(row[9]) << std::endl;
		}
	}

	//for (size_t i = 0; i < sptr_result->row_size(); i++)
	//{
	//	size_t j = 0;
	//	auto tem_0 = sptr_result->get<const int>(i, j);
	//	//auto tem_1 = sptr_result->get<std::string>(i, 1);
	//	//auto tem_2 = sptr_result->get<int>(i, 2);
	//	//auto tem_3 = sptr_result->get<int>(i, 3);
	//	//auto tem_4 = sptr_result->get<int>(i, 4);
	//	//auto tem_5 = sptr_result->get<int>(i, 5);
	//	//auto tem_6 = sptr_result->get<int>(i, 6);
	//	//auto tem_7 = sptr_result->get<int>(i, 7);
	//	//auto tem_8 = sptr_result->get<int>(i, 8);
	//}

}

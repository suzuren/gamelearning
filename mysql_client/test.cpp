#include "mysql.hpp"
#include "data_table.hpp"
#include <iostream>
int main()
{
    try{
        db::mysql mysql;

        mysql.connect("127.0.0.1",3306,"root","game123456","chess");

		auto sql = "\
        DROP TABLE IF EXISTS `article_detail`;\
        CREATE TABLE `article_detail` (\
        `id` BIGINT NOT NULL,\
        `parentid` BIGINT DEFAULT '0',\
        `title` varchar(255) DEFAULT '',\
        `content` varchar(255) DEFAULT '',\
        `updatetime` varchar(255) DEFAULT '',\
        PRIMARY KEY (`id`)\
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8;\
        ";
        mysql.execute(sql);
        //直接执行
        mysql.execute("INSERT INTO article_detail(id,parentid,title,content,updatetime) VALUES(1,2,'abc','abc','abc')");
        //stmt bind
        mysql.execute("INSERT INTO article_detail(id,parentid,title,content,updatetime) VALUES(?,?,?,?,?)",2,2,"abc","abc","abc");

        auto result = mysql.query<db::data_table>("select * from article_detail;");
        //for(auto& row : result)
        //{
        //   std::cout<<std::get<int64_t>(row[0])<<std::endl;
        //   std::cout<< std::get<int64_t>(row[1])<<std::endl;
        //   std::cout<< std::get<std::string>(row[2])<<std::endl;
        //   std::cout<< std::get<std::string>(row[3])<<std::endl;
        //   std::cout<< std::get<std::string>(row[4])<<std::endl;
        //}
    
    }catch(std::exception& e)
    {

    }
}
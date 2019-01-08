#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class MyClass
{
public:
	int count;
	string str1[1024];
	string str2[1024];

	bool empty() {
		return true;
	}
};

template <class T>
static int D_SQL_2(string oper, string tableName, T dataList, string &sql)
{
	string sqlTmp;
	if (oper == "insert") 
	{
		sql = "INSERT INTO " + tableName + " SET ";
		for (int i = 0; i < dataList.count; i++)
		{
			sqlTmp += dataList.str1[i] + "=" + dataList.str2[i] + ",";
		}

		sqlTmp.pop_back();
		sql += sqlTmp;
	}
	else if (oper == "select")
	{

		 if (dataList.empty())
		 {
		     sqlTmp = "*";
		 }
		 else
		 {
			 for (int i = 0; i < dataList.count; i++)
			 {
				 sqlTmp += dataList.str1[i];
			 }
		     sqlTmp.pop_back();
		 }
		 sql = "SELECT " + sqlTmp + " FROM " + tableName;
	}

	return 1;
}

int D_SQL_insert_2(string tableName, MyClass dataList, string &sql)
{
	return D_SQL_2("insert", tableName, dataList, sql);
}

template <class T, class M>
static int D_SQL(string oper, string tableName, T dataList, M conditions, string &sql)
{
    string sqlTmp;
    if (oper == "insert") {
        sql = "INSERT INTO " + tableName + " SET ";
        for (auto i : dataList) {
            sqlTmp += i.first + "=" + i.second + ",";
        }
        sqlTmp.pop_back();
        sql += sqlTmp;

    } else if (oper == "update") {
        sql = "UPDATE " + tableName + " SET ";
        for (auto i : dataList) {
            sqlTmp += i.first + "=" + i.second + ",";
        }
        sqlTmp.pop_back();
        sql += sqlTmp;

        if (!conditions.empty()) {
            sql += " WHERE ";
            for (auto i : conditions) {
                sqlTmp += i.first + "=" + i.second + ",";
            }
            sqlTmp.pop_back();
            sql += sqlTmp;
        }

    } else if (oper == "delete") {
        sql = "DELETE FROM " + tableName;

        if (!conditions.empty()) {
            sql += " WHERE ";
            for (auto i : conditions) {
                sqlTmp += i.first + "=" + i.second + ";";
            }
            sqlTmp.pop_back();
            sql += sqlTmp;
        }

    } else if (oper == "select") {

        // if (dataList.empty()) {
        //     sqlTmp = "*";
        // } else {
        //     for (auto &i : dataList) {
        //         sqlTmp += i + ",";
        //     }
        //     sqlTmp.pop_back();
        // }
        // sql = "SELECT " + sqlTmp + " FROM " + tableName;

        // if (!conditions.empty()) {
        //     for (auto i : conditions) {
        //         sqlTmp += i.first + "=" + i.second + ",";
        //     }
        //     sqlTmp.pop_back();
        //     sql += " WHERE " + sqlTmp;
        // }

    } else {
        return -1;
    }

    return 0;
}

int D_SQL_insert(string tableName, vector<pair<string, string>> dataList, string &sql)
{
	vector<pair<string, string>> temp;
    return D_SQL("insert", tableName, dataList, temp, sql);
}

int D_SQL_update(string tableName, vector<pair<string, string>> dataList,vector<pair<string, string>> conditions, string &sql)
{
    return D_SQL("update", tableName, dataList, conditions, sql);
}

int D_SQL_delete(string tableName, vector<pair<string, string>> conditions, string &sql)
{
	vector<pair<string, string>> temp;
    return D_SQL("delete", tableName, temp, conditions, sql);
}

//int D_SQL_select(string tableName, vector<string> dataList,vector<pair<string, string>> conditions, string &sql)
//{
//    return D_SQL("select", tableName, dataList, conditions, sql);
//}


#include "dataList.h"

int main()
{
    vector<pair<string, string>> dataList;
    string                       sql;
    //int                          ret = 0;

    dataList.push_back(pair<string, string>("retDesc", string("0")));
    dataList.push_back(pair<string, string>("retDesc", "SUCCESS"));

    D_SQL_insert("test", dataList, sql);
    cout << sql << endl;
    D_SQL_update("test", dataList, vector<pair<string, string>>{}, sql);
    cout << sql << endl;
    D_SQL_delete("test", dataList, sql);
    cout << sql << endl;

	unsigned int gametype = 3;
	unsigned int roomid = 4;
	unsigned int udice[3] = { 0 };

	DiceGameControlCard(gametype, roomid, udice);

    return 0;
}

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

sqlite3 *db;
int rc;

void start_db(){
	rc = sqlite3_open("user.db",&db);
	if(rc){
		fprintf(stderr,"Can't open db : %s\n",sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}

	// create 'user' table
	const char *create_user_sql = "create table if not exists user (id integer primary key, nickname text unique);";
	rc = sqlite3_exec(db, create_user_sql, 0, 0, 0);
	if(rc!=SQLITE_OK){
		fprintf(stderr, "SQL error : %s\n",sqlite3_errmsg(db));
	}

	// create 'message' table
	const char *create_message_sql = "create table if not exists message("
		"message_id integer primary key,"
		"message_text text,"
		"send_time timestamp,"
		"nickname text,"
		"foreign key (nickname) references user(nickname));";
	rc = sqlite3_exec(db,create_message_sql,0,0,0);
	if(rc!=SQLITE_OK){
		fprintf(stderr, "SQL error : %s\n",sqlite3_errmsg(db));
	}
	sqlite3_close(db);
}

int create_nickname(char *nickname){
	rc = sqlite3_open("user.db",&db);
	const char *insert_nickname = "insert into user (nickname) values (?);";
		sqlite3_stmt *stmt;
		rc = sqlite3_prepare_v2(db, insert_nickname, -1 ,&stmt, 0);
		if(rc!=SQLITE_OK){
			fprintf(stderr, "SQL error : %s\n",sqlite3_errmsg(db));
		}
		sqlite3_bind_text(stmt,1,nickname,-1,SQLITE_STATIC);
		rc=sqlite3_step(stmt);

		if(nickname[strlen(nickname)-1]!='\n'){
			printf("Nickname can be created with a maximun of 10 characters.\n");
			while(getchar()!='\n');
			return 1;
		}
		if(rc==SQLITE_CONSTRAINT){
			printf("Nickname already exists. Please choose a different nickname.\n");
			return 1;
		}
		else if(rc!=SQLITE_DONE){
			fprintf(stderr, "SQL error : %s\n",sqlite3_errmsg(db));
		}
		else{
			nickname[strlen(nickname)-1]='\0';
			printf("Hello. \" %s \"\n",nickname);
		}
		sqlite3_finalize(stmt);

		return 0;
		sqlite3_close(db);
}

void insert_message_log(char *message, char *timestamp, char *nickname){
	rc = sqlite3_open("user.db",&db);
	const char *insert_message = "insert into message(message_text, send_time, nickname) values (?,?,?);";
	sqlite3_stmt *stmt;
	rc = sqlite3_prepare_v2(db,insert_message,-1,&stmt,0);
	if(rc!=SQLITE_OK){
		printf("insert SQL error : %s\n",sqlite3_errmsg(db));
	}
	sqlite3_bind_text(stmt,1,message,-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt,2,timestamp,-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt,3,nickname,-1,SQLITE_STATIC);
	rc=sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}


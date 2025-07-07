/// 1.
// Search
			char buf[256];

			strlcpy(buf, LC_TEXT("서버에 치명적인 오류가 발생하여 자동으로 재부팅됩니다."), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("10초후 자동으로 접속이 종료되며,"), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("5분 후에 정상적으로 접속하실수 있습니다."), sizeof(buf));
			SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			SendNotice(LC_TEXT("서버에 치명적인 오류가 발생하여 자동으로 재부팅됩니다."));
			SendNotice(LC_TEXT("10초후 자동으로 접속이 종료되며,"));
			SendNotice(LC_TEXT("5분 후에 정상적으로 접속하실수 있습니다."));
#else
			char buf[256];

			strlcpy(buf, LC_TEXT("서버에 치명적인 오류가 발생하여 자동으로 재부팅됩니다."), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("10초후 자동으로 접속이 종료되며,"), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("5분 후에 정상적으로 접속하실수 있습니다."), sizeof(buf));
			SendNotice(buf);
#endif
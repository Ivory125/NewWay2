/// 1.
// Search
			char buf[256];

			strlcpy(buf, LC_TEXT("������ ġ������ ������ �߻��Ͽ� �ڵ����� ����õ˴ϴ�."), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("10���� �ڵ����� ������ ����Ǹ�,"), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("5�� �Ŀ� ���������� �����ϽǼ� �ֽ��ϴ�."), sizeof(buf));
			SendNotice(buf);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
			SendNotice(LC_TEXT("������ ġ������ ������ �߻��Ͽ� �ڵ����� ����õ˴ϴ�."));
			SendNotice(LC_TEXT("10���� �ڵ����� ������ ����Ǹ�,"));
			SendNotice(LC_TEXT("5�� �Ŀ� ���������� �����ϽǼ� �ֽ��ϴ�."));
#else
			char buf[256];

			strlcpy(buf, LC_TEXT("������ ġ������ ������ �߻��Ͽ� �ڵ����� ����õ˴ϴ�."), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("10���� �ڵ����� ������ ����Ǹ�,"), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("5�� �Ŀ� ���������� �����ϽǼ� �ֽ��ϴ�."), sizeof(buf));
			SendNotice(buf);
#endif
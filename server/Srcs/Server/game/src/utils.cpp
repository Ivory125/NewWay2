#include "stdafx.h"
#include <msl/msl.h>

static int global_time_gap = 0;

time_t get_global_time()
{
	return time(0) + global_time_gap;
}

void set_global_time(time_t t)
{
	global_time_gap = t - time(0);

	char time_str_buf[32];
	snprintf(time_str_buf, sizeof(time_str_buf), "%s", time_str(get_global_time()));

	sys_log(0, "GLOBAL_TIME: %s time_gap %d", time_str_buf, global_time_gap);
}

#include <iostream>
#include <string>
#include <iomanip>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

std::string mysql5_password(const std::string& input)
{
    CryptoPP::SHA1 sha1;
    CryptoPP::byte digest1[CryptoPP::SHA1::DIGESTSIZE];

    // First SHA1 hash
    sha1.Update(reinterpret_cast<const CryptoPP::byte*>(input.c_str()), input.length());
    sha1.Final(digest1);

    // Second SHA1 hash on the hex-encoded digest1
    std::string encoded1(reinterpret_cast<const char*>(digest1), CryptoPP::SHA1::DIGESTSIZE);
    std::string encoded2;

    CryptoPP::StringSource(encoded1, true,
        new CryptoPP::HashFilter(sha1,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(encoded2)
            )
        )
    );

    // Uppercase and prepend '*'
    std::string password = "*" + encoded2;
    for (char& c : password) {
        c = std::toupper(c);
    }

    return password;
}

std::string mysql_hash_password(const char* tmp_pwd)
{
	return mysql5_password(tmp_pwd);
}

int dice(int number, int size)
{
	int sum = 0, val;

	if (size <= 0 || number <= 0)
		return (0);

	while (number)
	{
		val = msl::random_int(0, size);
		sum += val;
		--number;
	}

	return (sum);
}

size_t str_lower(const char * src, char * dest, size_t dest_size)
{
	size_t len = 0;

	if (!dest || dest_size == 0)
		return len;

	if (!src)
	{
		*dest = '\0';
		return len;
	}

	--dest_size;

	while (*src && len < dest_size)
	{
		*dest = LOWER(*src);

		++src;
		++dest;
		++len;
	}

	*dest = '\0';
	return len;
}

void skip_spaces(const char **string)
{
	for (; **string != '\0' && isnhspace(**string); ++(*string));
}

const char *one_argument(const char *argument, char *first_arg, size_t first_size)
{
	char mark = FALSE;
	size_t first_len = 0;

	if (!argument || 0 == first_size)
	{
		sys_err("one_argument received a NULL pointer!");
		*first_arg = '\0';
		return NULL;
	}

	--first_size;

	skip_spaces(&argument);

	while (*argument && first_len < first_size)
	{
		if (*argument == '\"')
		{
			mark = !mark;
			++argument;
			continue;
		}

		if (!mark && isnhspace(*argument))
			break;

		*(first_arg++) = *argument;
		++argument;
		++first_len;
	}

	*first_arg = '\0';

	skip_spaces(&argument);
	return (argument);
}

const char *two_arguments(const char *argument, char *first_arg, size_t first_size, char *second_arg, size_t second_size)
{
	return (one_argument(one_argument(argument, first_arg, first_size), second_arg, second_size));
}

const char* three_arguments(const char* argument, char* first_arg, size_t first_size, char* second_arg, size_t second_size, char* third_arg, size_t third_size)
{
	return (one_argument(one_argument(one_argument(argument, first_arg, first_size), second_arg, second_size), third_arg, third_size));
}

#ifdef ADMINPANEL_MOD_CREATE_ITEM_ASLAN
const char* adminpanel_create_item_arguments(const char* argument, 
	char* vnum_arg, size_t vnum_size, 
	char* count_arg, size_t count_size,
	char* socket0_arg, size_t socket0_size, char* socket1_arg, size_t socket1_size, char* socket2_arg, size_t socket2_size,
	char* applytype0_arg, size_t applytype0_size, char* applyvalue0_arg, size_t applyvalue0_size,
	char* applytype1_arg, size_t applytype1_size, char* applyvalue1_arg, size_t applyvalue1_size,
	char* applytype2_arg, size_t applytype2_size, char* applyvalue2_arg, size_t applyvalue2_size,
	char* applytype3_arg, size_t applytype3_size, char* applyvalue3_arg, size_t applyvalue3_size,
	char* applytype4_arg, size_t applytype4_size, char* applyvalue4_arg, size_t applyvalue4_size,
	char* applytype5_arg, size_t applytype5_size, char* applyvalue5_arg, size_t applyvalue5_size,
	char* applytype6_arg, size_t applytype6_size, char* applyvalue6_arg, size_t applyvalue6_size,
	char* playername_arg, size_t playername_size
)
{
	return (one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(one_argument(argument,
			vnum_arg, vnum_size), 
			count_arg, count_size),
			socket0_arg, socket0_size), socket1_arg, socket1_size), socket2_arg, socket2_size),
			applytype0_arg, applytype0_size), applyvalue0_arg, applyvalue0_size),
			applytype1_arg, applytype1_size), applyvalue1_arg, applyvalue1_size),
			applytype2_arg, applytype2_size), applyvalue2_arg, applyvalue2_size),
			applytype3_arg, applytype3_size), applyvalue3_arg, applyvalue3_size),
			applytype4_arg, applytype4_size), applyvalue4_arg, applyvalue4_size),
			applytype5_arg, applytype5_size), applyvalue5_arg, applyvalue5_size),
			applytype6_arg, applytype6_size), applyvalue6_arg, applyvalue6_size),
			playername_arg, playername_size));
}
#endif

const char *first_cmd(const char *argument, char *first_arg, size_t first_arg_size, size_t *first_arg_len_result)
{
	size_t cur_len = 0;
	skip_spaces(&argument);

	first_arg_size -= 1;

	while (*argument && !isnhspace(*argument) && cur_len < first_arg_size)
	{
		*(first_arg++) = LOWER(*argument);
		++argument;
		++cur_len;
	}

	*first_arg_len_result = cur_len;
	*first_arg = '\0';
	return (argument);
}

int CalculateDuration(int iSpd, int iDur)
{
	int i = 100 - iSpd;

	if (i > 0)
		i = 100 + i;
	else if (i < 0)
		i = 10000 / (100 - i);
	else
		i = 100;

	return iDur * i / 100;
}

double uniform_random(double a, double b)
{
	return msl::random_real(a, b);
}

float gauss_random(float avg, float sigma)
{
	static bool haveNextGaussian = false;
	static float nextGaussian = 0.0f;

	if (haveNextGaussian)
	{
		haveNextGaussian = false;
		return nextGaussian * sigma + avg;
	}
	else
	{
		double v1, v2, s;
		do {
			//v1 = 2 * nextDouble() - 1;   // between -1.0 and 1.0
			//v2 = 2 * nextDouble() - 1;   // between -1.0 and 1.0
			v1 = uniform_random(-1.f, 1.f);
			v2 = uniform_random(-1.f, 1.f);
			s = v1 * v1 + v2 * v2;
		} while (s >= 1.f || fabs(s) < FLT_EPSILON);
		double multiplier = sqrtf(-2 * logf(s)/s);
		nextGaussian = v2 * multiplier;
		haveNextGaussian = true;
		return v1 * multiplier * sigma + avg;
	}
}

int parse_time_str(const char* str)
{
	int tmp = 0;
	int secs = 0;

	while (*str != 0)
	{
		switch (*str)
		{
			case 'm':
			case 'M':
				secs += tmp * 60;
				tmp = 0;
				break;

			case 'h':
			case 'H':
				secs += tmp * 3600;
				tmp = 0;
				break;

			case 'd':
			case 'D':
				secs += tmp * 86400;
				tmp = 0;
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				tmp *= 10;
				tmp += (*str) - '0';
				break;

			case 's':
			case 'S':
				secs += tmp;
				tmp = 0;
				break;
			default:
				return -1;
		}
		++str;
	}

	return secs + tmp;
}

bool WildCaseCmp(const char *w, const char *s)
{
	for (;;)
	{
		switch(*w)
		{
			case '*':
				if ('\0' == w[1])
					return true;
				{
					for (size_t i = 0; i <= strlen(s); ++i)
					{
						if (true == WildCaseCmp(w + 1, s + i))
							return true;
					}
				}
				return false;

			case '?':
				if ('\0' == *s)
					return false;

				++w;
				++s;
				break;

			default:
				if (*w != *s)
				{
					if (tolower(*w) != tolower(*s))
						return false;
				}

				if ('\0' == *w)
					return true;

				++w;
				++s;
				break;
		}
	}

	return false;
}

#include <string>
#include <sstream>
#include <iostream>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

std::string num_with_point(int n)
{
	std::string ans = "";
    //std::string num = std::__cxx11::to_string(n);
	std::string num = patch::to_string(n);
    int count = 0;
  
    for (int i = num.size() - 1; i >= 0; i--) {
        count++;
        ans.push_back(num[i]);
        if (count == 3) {
            ans.push_back('.');
            count = 0;
        }
    }
    reverse(ans.begin(), ans.end());
	
    if (ans.size() % 4 == 0) {
        ans.erase(ans.begin());
    }

    return ans;
}
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e

#define _FISHINO_PRINT_BUF_SIZE 16
size_t FishinoClient::print(const __FlashStringHelper *s)
{
	PGM_P p = reinterpret_cast<PGM_P>(s);
	char buf[_FISHINO_PRINT_BUF_SIZE];
	uint8_t iBuf = 0;
	size_t n = 0;
	while (1)
	{
		unsigned char c = pgm_read_byte(p++);
		if(c == 0)
		{
			if(iBuf > 0)
				n += write(buf, iBuf);
			return n;
		}
		buf[iBuf++] = c;
		if(iBuf >= _FISHINO_PRINT_BUF_SIZE)
		{
			n +=write(buf, _FISHINO_PRINT_BUF_SIZE);
			iBuf = 0;
		}
	}
}

size_t FishinoClient::println(const __FlashStringHelper *s)
{
	PGM_P p = reinterpret_cast<PGM_P>(s);
	char buf[_FISHINO_PRINT_BUF_SIZE];
	uint8_t iBuf = 0;
	size_t n = 0;
	while (1)
	{
		unsigned char c = pgm_read_byte(p++);
		if(c == 0)
		{
			if(iBuf > 0)
				n += write(buf, iBuf);
			break;
		}
		buf[iBuf++] = c;
		if(iBuf >= _FISHINO_PRINT_BUF_SIZE)
		{
			n +=write(buf, _FISHINO_PRINT_BUF_SIZE);
			iBuf = 0;
		}
	}
	
	if(iBuf >= _FISHINO_PRINT_BUF_SIZE - 1)
	{
		n +=write(buf, iBuf);
		iBuf = 0;
	}
	buf[iBuf++] = '\r';
	buf[iBuf++] = '\n';
	n +=write(buf, iBuf);
	return n;
}


enum _portType
{
	NONE = 0,
	MONO = 1,
	STEREO,
	SURROUND
};

typedef _portType portType;


portType toPortType( char in, bool strict = true, bool force51Stereo = false )
{
	portType type = NONE;

	switch( in )
	{
		case 'L':
		case '7':
			type = STEREO;
			break;

		case 'R':
		case '8':
			if( strict )
				type = NONE;
			else
				type = STEREO;
			break;

		case 'M':
			type = MONO;
			break;

		case '1':
			if( force51Stereo )
				type = STEREO;
			else
				type = SURROUND;
			break;

		case '2':
		case '4':
		case '6':
			if( strict )
				type = NONE;
			else
				type = SURROUND;
			break;

		case '3':
		case '5':
			if( force51Stereo )
				type = STEREO;
			else if( strict )
				type = NONE;
			else
				type = SURROUND;
	}
	return type;
}

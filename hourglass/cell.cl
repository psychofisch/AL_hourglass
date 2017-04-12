__kernel void cell(
	__global uint *elements,
	const int size_x,
	const int size_y,
	const int init,
	const int r
)
{
	int tidX = get_global_id(0);
	int tidY = get_global_id(1);

	/*if (init == 0)
	{
		if (tidX % 2 == 0 || tidY % 2 == 0)
			return;
	}
	else {
		if (tidX+1 % 2 == 0 || tidY+1 % 2 == 0)
			return;
	}*/

	if (tidX < size_x*0.5 && tidY < size_y*0.5)
	{
		tidX *= 2;
		tidY *= 2;

		if (init == 1)
		{
			tidX++;
			tidY++;
		}

		int pos[4];
		pos[0] = (tidX + (tidY * size_x));
		pos[1] = ((tidX + 1) + (tidY * size_x));
		pos[2] = (tidX + ((tidY + 1) * size_x));
		pos[3] = ((tidX + 1) + ((tidY + 1) * size_x));

		//	x | o		o | o
		//	- - -	->	- - -
		//	o | o		x | o
		if (elements[pos[0]] == 0xffffffff && elements[pos[1]] == 0xff000000 && elements[pos[2]] == 0xff000000 && elements[pos[3]] == 0xff000000)
		{
			elements[pos[0]] = 0xff000000;
			elements[pos[2]] = 0xffffffff;
		}
		//	o | x		o | o
		//	- - -	->	- - -
		//	o | o		o | x
		else if (elements[pos[0]] == 0xff000000 && elements[pos[1]] == 0xffffffff && elements[pos[2]] == 0xff000000 && elements[pos[3]] == 0xff000000)
		{
			elements[pos[1]] = 0xff000000;
			elements[pos[3]] = 0xffffffff;
		}
		//	x | o		o | o	&	o | x		o | o	&	o | x		o | o	&	x | o		o | o
		//	- - -	->	- - -	&	- - -	->	- - -	& 	- - -	->	- - -	&	- - -	->	- - -
		//	x | o		x | x	&	o | x		x | x	&	x | o		x | x	&	o | x		x | x
		else if (elements[pos[0]] == 0xffffffff && elements[pos[1]] == 0xff000000 && elements[pos[2]] == 0xffffffff && elements[pos[3]] == 0xff000000 ||
			elements[pos[0]] == 0xff000000 && elements[pos[1]] == 0xffffffff && elements[pos[2]] == 0xff000000 && elements[pos[3]] == 0xffffffff ||
			elements[pos[0]] == 0xff000000 && elements[pos[1]] == 0xffffffff && elements[pos[2]] == 0xffffffff && elements[pos[3]] == 0xff000000 ||
			elements[pos[0]] == 0xffffffff && elements[pos[1]] == 0xff000000 && elements[pos[2]] == 0xff000000 && elements[pos[3]] == 0xffffffff)
		{
			elements[pos[0]] = elements[pos[1]] = 0xff000000;
			elements[pos[2]] = elements[pos[3]] = 0xffffffff;
		}
		//	x | x		o | x
		//	- - -	->	- - -
		//	o | x		x | x
		else if (elements[pos[0]] == 0xffffffff && elements[pos[1]] == 0xffffffff && elements[pos[2]] == 0xff000000 && elements[pos[3]] == 0xffffffff)
		{
			elements[pos[0]] = 0xff000000;
			elements[pos[2]] = 0xffffffff;
		}
		//	x | x		x | o
		//	- - -	->	- - -
		//	x | o		x | x
		else if (elements[pos[0]] == 0xffffffff && elements[pos[1]] == 0xffffffff && elements[pos[2]] == 0xffffffff && elements[pos[3]] == 0xff000000)
		{
			elements[pos[1]] = 0xff000000;
			elements[pos[3]] = 0xffffffff;
		}
		//	x | x		o | o		x | x
		//	- - -	->	- - -	OR	- - -
		//	o | o		x | x		o | o
		else if (elements[pos[0]] == 0xffffffff && elements[pos[1]] == 0xffffffff && elements[pos[2]] == 0xff000000 && elements[pos[3]] == 0xff000000)
		{
			if (r)
			{
				elements[pos[0]] = elements[pos[1]] = 0xff000000;
				elements[pos[2]] = elements[pos[3]] = 0xffffffff;
			}
		}
	}
}

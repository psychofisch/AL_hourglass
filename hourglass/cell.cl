__kernel void cell(
	__global uint *elements,
	const int size_x,
	const int size_y
)
{
	int tidX = get_global_id(0);
	int tidY = get_global_id(1);

	if (tidX < size_x && tidY < size_y)
	{
		uint pos1 = ((tidY * size_x) + tidX) * 4;
		uint pos2 = (((tidY + 1) * size_x) + tidX) * 4;
		//
		//uint fields[4];
		//fields[0] = elements[pos + 0];
		//fields[1] = elements[pos + 1];
		//fields[2] = elements[pos + 2];
		//fields[3] = elements[pos + 3];

		////elements[pos + 0] = (char[4]) {255, 0, 0, 255};
		////elements[pos + 1] = 0;
		////elements[pos + 2] = 0;
		////elements[pos + 3] = 0;
		//printf("%d:%d -> %d\n", tidX, tidY, pos1);
		elements[pos1 + 0] = 0xff00ff01;
		elements[pos1 + 1] = 0xff00ff01;
		elements[pos2 + 0] = 0xff00ff01;
		elements[pos2 + 1] = 0xff00ff01;
	}
}

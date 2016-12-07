static bool gizProtocolGetOnePacket( rb_t *rb, uint8 *data, uint8 *len )
{	
	static uint8 lastData = 0;
	static uint8 *buffdata = data;
	
	uint8 nowData = 0;
	uint8 dataLen = buffdata - data;
	uint8 packetLen = 0;
	
	while( rbRead( rb, &nowData, 1 ) == 0 )
	{
		if( lastData == 0xFF && nowData == 0xFF )
		{
			buffdata = data;
			
			*buffdata = 0xFF;
			buffdata++;
			
			*buffdata = 0xFF;
			buffdata++;
			
			dataLen = 2;
		}
		else if( lastData == 0xFF && nowData == 0x55 )
		{
		}
		else
		{
			if( dataLen >= 2 )
			{
				*buffdata = nowData;
				buffdata++;
				
				dataLen = buffdata - data;
				if( dataLen > 4 )
				{
					packetLen = gizProtocolExchangeBytes(((protocolHead_t *)buffdata)->len)+4;
					if( dataLen == packetLen )
					{
						if( gizProtocolSum(buffdata, packetLen) == buffdata[packetLen-1] )
						{
							*len = packetLen;
							lastData = 0;
							return true;
						}
						else
						{
							buffdata = data;
						}
					}
				}
			}
			else
			{
				buffdata = data;
			}
		}
		lastData = rdData;
	}
	
	return false;
}
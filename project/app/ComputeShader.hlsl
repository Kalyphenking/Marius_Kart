[numthreads(1, 1, 1)]

PSDepthIn VSDepthMain(VSDepthIn _Input)
{
	float4 WSPosition;
	PSDepthIn Output;
	WSPosition = mul(g_WorldMatrix, float4(_Input.m_Position, 1.0f));
	Output.m_Position = mul(g_VSMainViewProjectionMatrix, WSPosition);
	Output.m_Depth = Output.m_Position.w / g_CameraFar;
	return Output;
}
float4 PSDepthMain(PSDepthIn _Input) : SV_Target
{
return _Input.m_Depth;
}



void main( uint3 DTid : SV_DispatchThreadID )
{
}
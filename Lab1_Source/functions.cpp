using namespace std;
using glm::vec3;
using glm::mat3;

void Interpolate( float a, float b, vector<float> & result ){
	int size = result.size() - 1;
	float ab_interval = (a - b) / size;
	if (size == 1){
		result[0] = (a + b)/2;
	}
	else {
		for( int i=0; i<result.size(); ++i )
			result[i] = a - i*ab_interval;
	}
}

void Interpolate( vec3 a, vec3 b, vector<vec3>& result ){
	int size = result.size() - 1;
	vec3 norn;
	float x_interval = (a.x - b.x) / size;
	float y_interval = (a.y - b.y) / size;
	float z_interval = (a.z - b.z) / size;
	if (size == 1){
		result[0].x = (a.x + b.x) / 2;
		result[0].y = (a.y + b.y) / 2;
		result[0].z = (a.z + b.z) / 2;
	}
	else {
		for( int i=0; i<result.size(); ++i ) {
			result[i].x = a.x - i*x_interval;
			result[i].y = a.y - i*y_interval;
			result[i].z = a.z - i*z_interval;
		}
	}
}

void Interpolate_function(){
	vector<float> result( 10 ); // Create a vector width 10 floats
	Interpolate( -4, 5, result ); // Fill it with interpolated values
	for( int i=0; i<result.size(); ++i )
	    cout << result[i] << " "; // Print the result to the terminal
	cout << "\n";
}

void InterpolateVec3_function(){
	vector<vec3> result( 4 );
	vec3 a(1,4,9.2);
	vec3 b(4,1,9.8);
	Interpolate( a, b, result );
	for( int i=0; i<result.size(); ++i )
	{
	    cout << "( "
	 << result[i].x << ", "
	 << result[i].y << ", "
	 << result[i].z << " ) ";
	}
}


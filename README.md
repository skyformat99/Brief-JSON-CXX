# Brief-JSON-CXX
C++ edition of brief-JSON

JAVA Edition: https://github.com/fan-wenjie/Brief-JSON

API:

briefJson::deserializer :Parse Json text to json object value combined by std::vector and std::map etc.;

briefJson::serializer :Create Json text from json object value combined by std::vector and std::map etc.;

briefJson::dispose : Dispose json object value and free memory

Typedef:

briefJson::value_t : Json object value combined by point of data and type_info

briefJson::exception_t :Exception thrown when parsing an illegal json

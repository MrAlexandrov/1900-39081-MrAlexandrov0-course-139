import aiohttp
import pytest

async def create_multipart_data(email=None, password=None):
    with aiohttp.MultipartWriter('form-data') as data:
        if email:
            payload = aiohttp.payload.StringPayload(email)
            payload.set_content_disposition('form-data', name='email')
            data.append_payload(payload)
        if password:
            payload = aiohttp.payload.StringPayload(password)
            payload.set_content_disposition('form-data', name='password')
            data.append_payload(payload)
    return data

def create_headers(data):
    return {
        'Content-Type': f'multipart/form-data; boundary={data.boundary}',
    }
import aiohttp

import pytest

# Start the tests via `make test-debug` or `make test-release`


async def test_multipart_form_data(service_client):
    with aiohttp.MultipartWriter('form-data') as data:
        payload = aiohttp.payload.StringPayload('test')
        payload.set_content_disposition('form-data', name='email')
        data.append_payload(payload)

    headers = {
        'Content-Type': 'multipart/form-data; boundary=' + data.boundary,
    }

    response = await service_client.post(
        '/register',
        data=data,
        headers=headers
    )

    assert response.status == 200

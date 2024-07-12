from .helpers import *

async def test_add_bookmark(service_client):
    data = await create_multipart_data(email='me@ya.ru', password='pswd')
    headers = create_headers(data)
    response = await service_client.post('/register', data=data, headers=headers)
    assert response.status == 200

    data = await create_multipart_data(email='me@ya.ru', password='pswd')
    headers = create_headers(data)
    response = await service_client.post('/login', data=data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    user_id = json_response['id']

    bookmark_data = {
        "url": "https://ya.ru",
        "title": "Yandex",
        "tag": "Ya"
    }
    headers = {
        'Content-Type': 'application/json',
        'X-Ya-User-Ticket': user_id
    }
    response = await service_client.post('/v1/bookmarks', json=bookmark_data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert "url" in json_response
    assert "title" in json_response
    assert "created_ts" in json_response

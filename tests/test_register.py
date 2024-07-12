from .helpers import *

async def test_register_missing_email(service_client):
    data = await create_multipart_data(password='pswd')
    headers = create_headers(data)
    response = await service_client.post('/register', data=data, headers=headers)
    assert response.status == 400

async def test_register_missing_password(service_client):
    data = await create_multipart_data(email='me@ya.ru')
    headers = create_headers(data)
    response = await service_client.post('/register', data=data, headers=headers)
    assert response.status == 400

async def test_register_missing_parametres(service_client):
    data = await create_multipart_data()
    headers = create_headers(data)
    response = await service_client.post('/register', data=data, headers=headers)
    assert response.status == 400

async def test_register_success(service_client):
    data = await create_multipart_data(email='me@ya.ru', password='pswd')
    headers = create_headers(data)
    response = await service_client.post('/register', data=data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response